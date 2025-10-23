#include "graphicsarcitem.h"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QtMath>

namespace Graphics {

namespace {

auto calculateFinalArc(const QPolygonF &ply) -> Arc
{
    if (ply.size() != 4) {
        return Arc{};
    }

    const QPointF &A = ply[0];
    const QPointF &B = ply[1];
    const QPointF &C = ply[2];
    const QPointF &D = ply[3];

    // 计算圆心和内半径（使用前三个点）
    QPointF center;
    double innerRadius = 0;
    if (!Utils::calculateCircle(ply.mid(0, 3), center, innerRadius)) {
        return Arc{};
    }

    // 计算外半径（第四个点到圆心的距离）
    const double outerRadius = QLineF(center, D).length();

    // 计算角度
    const double angleA = QLineF(center, A).angle();
    const double angleB = QLineF(center, B).angle();
    const double angleC = QLineF(center, C).angle();

    // 确定圆弧方向
    const bool isCounterClockwise = Utils::isPointOnCounterClockwiseSide(A, B, C, center);

    // 计算起始角度和扫过角度
    double startAngle, sweepAngle;
    Utils::calculateArcAngles(angleA, angleB, angleC, isCounterClockwise, startAngle, sweepAngle);

    // 计算结束角度
    double endAngle = startAngle + sweepAngle;

    if (sweepAngle <= 0) {
        auto startAngle_ = endAngle + 360;
        auto endAngle_ = startAngle + 360;
        startAngle = startAngle_;
        endAngle = endAngle_;
    }

    return Arc{center,
               qMin(innerRadius, outerRadius),
               qMax(innerRadius, outerRadius),
               startAngle,
               endAngle};
}

} // namespace

auto Arc::isValid(double margin) const -> bool
{
    double minLen = margin * qSqrt(2) / 2;
    return maxRadius > minRadius && minRadius >= minLen && maxRadius - minRadius >= minLen;
}

auto Arc::controlPoints() const -> QPolygonF
{
    return {Utils::pointFromCenter(center, minRadius, startAngle),
            Utils::pointFromCenter(center, minRadius, endAngle),
            Utils::pointFromCenter(center, minRadius, (startAngle + endAngle) / 2.0),
            Utils::pointFromCenter(center, maxRadius, (startAngle + endAngle) / 2.0)};
}

class GraphicsArcItem::GraphicsArcItemPrivate
{
public:
    enum class MouseEdgeRegion : int {
        NoSelection,
        InnerEdge,
        OuterEdge,
        StartAngleSide,
        EndAngleSide
    };

    explicit GraphicsArcItemPrivate(GraphicsArcItem *q)
        : q_ptr(q)
    {}

    GraphicsArcItem *q_ptr;

    Arc arch;
    QPainterPath arcPath;
    QPainterPath cachePath;
    MouseEdgeRegion mouseRegion = MouseEdgeRegion::NoSelection;
};

GraphicsArcItem::GraphicsArcItem(QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsArcItemPrivate(this))
{}

GraphicsArcItem::GraphicsArcItem(const Arc &arc, QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsArcItemPrivate(this))
{
    setArc(arc);
}

GraphicsArcItem::~GraphicsArcItem() {}

auto GraphicsArcItem::setArc(const Arc &arc) -> bool
{
    if (!arc.isValid(margin())) {
        return false;
    }

    auto anchorPoints = arc.controlPoints();
    if (!Utils::calculateAllArc(anchorPoints, d_ptr->arcPath, margin())) {
        return false;
    }

    auto sceneRect = scene()->sceneRect();
    QPolygonF pts = d_ptr->arcPath.toFillPolygon() + anchorPoints;
    double addLen = margin() * qSqrt(2) / 2;
    auto rect = pts.boundingRect().adjusted(-addLen, -addLen, addLen, addLen);
    if (!sceneRect.contains(rect)) {
        return false;
    }

    prepareGeometryChange();
    d_ptr->arch = arc;
    geometryCache()->setGeometryData(anchorPoints,
                                     Utils::createBoundingRect(pts, 0),
                                     d_ptr->arcPath);

    return true;
}

auto GraphicsArcItem::arch() const -> Arc
{
    return d_ptr->arch;
}

void GraphicsArcItem::drawContent(QPainter *painter)
{
    painter->drawPath(isValid() ? d_ptr->arcPath : d_ptr->cachePath);
}

void GraphicsArcItem::pointsChanged(const QPolygonF &ply)
{
    if (ply.size() == 3) {
        double deltaAngle = QLineF(ply[0], ply[1]).angleTo(QLineF(ply[0], ply[2]));
        if (deltaAngle < 0.00001 || deltaAngle > 355.99999) {
            return;
        }
    }

    auto sceneRect = scene()->sceneRect();

    switch (ply.size()) {
    case 1:
    case 2: geometryCache()->setControlPoints(ply); break;
    case 3: {
        if (!Utils::calculateHalfArc(ply, d_ptr->cachePath)) {
            return;
        }
        QPolygonF polygon = d_ptr->cachePath.toFillPolygon() + ply;
        if (!sceneRect.contains(polygon.boundingRect())) {
            return;
        }
        geometryCache()->setControlPoints(ply);
    } break;
    case 4: {
        if (!setArc(calculateFinalArc(ply))) {
            return;
        }
    } break;
    default: return;
    }

    update();
}

void GraphicsArcItem::updateHoverPreview(const QPointF &scenePos)
{
    auto controlPoints = geometryCache()->controlPoints();
    auto size = controlPoints.size();
    if (size < 2 || size > 3) {
        return;
    }

    controlPoints.append(scenePos);
    size = controlPoints.size();

    switch (size) {
    case 3:
        // QPainterPath::arcTo: Adding point with invalid coordinates, ignoring call
        if (Utils::distance(controlPoints[1], controlPoints[2]) < margin()) {
            return;
        }
        Utils::calculateHalfArc(controlPoints, d_ptr->cachePath);
        break;
    case 4: Utils::calculateAllArc(controlPoints, d_ptr->cachePath, margin()); break;
    default: return;
    }

    update();
}

GraphicsBasicItem::MouseRegion GraphicsArcItem::detectEdgeRegion(const QPointF &scenePos)
{
    const double edgeMargin = margin() / 2.0;
    const Arc &arc = d_ptr->arch;

    // 计算点到圆心的距离
    const double distanceToCenter = Utils::distance(scenePos, arc.center);

    // 分别检查内外圆弧边缘 - 更清晰的逻辑
    if (qAbs(distanceToCenter - arc.minRadius) < edgeMargin) {
        d_ptr->mouseRegion = GraphicsArcItemPrivate::MouseEdgeRegion::InnerEdge;
        setMyCursor(arc.center, scenePos);
        setMouseRegion(GraphicsBasicItem::MouseRegion::EdgeArea);
        return GraphicsBasicItem::MouseRegion::EdgeArea;
    }

    if (qAbs(distanceToCenter - arc.maxRadius) < edgeMargin) {
        d_ptr->mouseRegion = GraphicsArcItemPrivate::MouseEdgeRegion::OuterEdge;
        setMyCursor(arc.center, scenePos);
        setMouseRegion(GraphicsBasicItem::MouseRegion::EdgeArea);
        return GraphicsBasicItem::MouseRegion::EdgeArea;
    }

    // 计算侧边端点
    const QPointF innerStart = Utils::pointFromCenter(arc.center, arc.minRadius, arc.startAngle);
    const QPointF outerStart = Utils::pointFromCenter(arc.center, arc.maxRadius, arc.startAngle);

    // 分别检查两个侧边
    const QLineF startEdge(innerStart, outerStart);
    if (Utils::isPointNearEdge(scenePos, startEdge, edgeMargin)) {
        d_ptr->mouseRegion = GraphicsArcItemPrivate::MouseEdgeRegion::StartAngleSide;
        setCursor(Utils::cursorForDirection(startEdge.angle()));
        setMouseRegion(GraphicsBasicItem::MouseRegion::EdgeArea);
        return GraphicsBasicItem::MouseRegion::EdgeArea;
    }

    const QPointF innerEnd = Utils::pointFromCenter(arc.center, arc.minRadius, arc.endAngle);
    const QPointF outerEnd = Utils::pointFromCenter(arc.center, arc.maxRadius, arc.endAngle);

    const QLineF endEdge(innerEnd, outerEnd);
    if (Utils::isPointNearEdge(scenePos, endEdge, edgeMargin)) {
        d_ptr->mouseRegion = GraphicsArcItemPrivate::MouseEdgeRegion::EndAngleSide;
        setCursor(Utils::cursorForDirection(endEdge.angle()));
        setMouseRegion(GraphicsBasicItem::MouseRegion::EdgeArea);
        return GraphicsBasicItem::MouseRegion::EdgeArea;
    }

    // 不在任何区域
    d_ptr->mouseRegion = GraphicsArcItemPrivate::MouseEdgeRegion::NoSelection;
    setMouseRegion(GraphicsBasicItem::MouseRegion::NoSelection);
    return GraphicsBasicItem::MouseRegion::NoSelection;
}

void GraphicsArcItem::handleMouseMoveEvent(const QPointF &scenePos,
                                           const QPointF &clickedPos,
                                           const QPointF delta)
{
    auto controlPoints = geometryCache()->controlPoints();
    auto arch = d_ptr->arch;

    switch (mouseRegion()) {
    case GraphicsBasicItem::MouseRegion::EntireShape: controlPoints.translate(delta); break;
    case GraphicsBasicItem::MouseRegion::AnchorPoint:
        controlPoints[hoveredDotIndex()] += delta;
        break;
    case GraphicsBasicItem::MouseRegion::EdgeArea: {
        auto distance = Utils::distance(arch.center, scenePos);
        switch (d_ptr->mouseRegion) {
        case GraphicsArcItemPrivate::MouseEdgeRegion::InnerEdge:
            setMyCursor(arch.center, scenePos);
            for (int i = 0; i < 3; ++i) {
                controlPoints[i] = Utils::pointAtDistance(arch.center, controlPoints[i], distance);
            }
            break;
        case GraphicsArcItemPrivate::MouseEdgeRegion::OuterEdge:
            setMyCursor(arch.center, scenePos);
            controlPoints[3] = Utils::pointAtDistance(arch.center, controlPoints[3], distance);
            break;
        case GraphicsArcItemPrivate::MouseEdgeRegion::StartAngleSide:
        case GraphicsArcItemPrivate::MouseEdgeRegion::EndAngleSide: {
            auto &targetAngle = (d_ptr->mouseRegion
                                 == GraphicsArcItemPrivate::MouseEdgeRegion::StartAngleSide)
                                    ? arch.startAngle
                                    : arch.endAngle;
            targetAngle = QLineF(arch.center, scenePos).angle();
            setCursor(Utils::cursorForDirection(targetAngle));
            while (arch.startAngle > arch.endAngle) {
                arch.endAngle += 360;
            }
            if (arch.endAngle - arch.startAngle > 360) {
                arch.endAngle -= 360;
            }
            controlPoints = arch.controlPoints();
        } break;
        default: return;
        }
        break;
    }
    default: return;
    }

    pointsChanged(controlPoints);
}

} // namespace Graphics

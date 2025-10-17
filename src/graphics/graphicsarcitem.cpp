#include "graphicsarcitem.h"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QtMath>

namespace Graphics {

inline auto calculateFinalArc(const QPolygonF &ply) -> Arc
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
    explicit GraphicsArcItemPrivate(GraphicsArcItem *q)
        : q_ptr(q)
    {}

    GraphicsArcItem *q_ptr;

    Arc arch;
    QPainterPath arcPath;
    QPainterPath cachePath;
    GraphicsArcItem::MouseRegion mouseRegion = GraphicsArcItem::None;
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
    geometryCache()->setControlPoints(anchorPoints,
                                      Utils::createBoundingRect(pts, 0),
                                      d_ptr->arcPath);

    return true;
}

auto GraphicsArcItem::arch() const -> Arc
{
    return d_ptr->arch;
}

auto GraphicsArcItem::type() const -> int
{
    return GraphicsBasicItem::Shape::ARC;
}

inline auto lineSetLength(const QPointF p1, const QPointF p2, const double len) -> QPointF
{
    QLineF line(p1, p2);
    line.setLength(len);
    return line.p2();
}

void GraphicsArcItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!isValid()) {
        return;
    }
    if (!isSelected()) {
        setSelected(true);
    }

    QPointF point = event->scenePos();
    QPointF dp = point - clickedPos();
    setClickedPos(event->scenePos());
    auto pts_tmp = geometryCache()->controlPoints();
    double distance = Utils::distance(d_ptr->arch.center, point);

    switch (mouseRegion()) {
    case GraphicsBasicItem::MouseRegion::All: pts_tmp.translate(dp); break;
    case GraphicsBasicItem::MouseRegion::None: {
        switch (d_ptr->mouseRegion) {
        case InEdge0:
            setMyCursor(d_ptr->arch.center, event->scenePos());
            for (int i = 0; i < 3; ++i) {
                pts_tmp[i] = lineSetLength(d_ptr->arch.center, pts_tmp[i], distance);
            }
            break;
        case InEdge1:
            setMyCursor(d_ptr->arch.center, event->scenePos());
            pts_tmp[3] = lineSetLength(d_ptr->arch.center, pts_tmp[3], distance);
            break;
        case InEdgeH:
        case InEdgeL: {
            Arc arch = d_ptr->arch;
            if (d_ptr->mouseRegion == InEdgeL) {
                arch.startAngle = QLineF(arch.center, event->scenePos()).angle();
                setCursor(Utils::cursorForDirection(arch.startAngle));
            } else {
                arch.endAngle = QLineF(arch.center, event->scenePos()).angle();
                setCursor(Utils::cursorForDirection(arch.endAngle));
            }
            while (arch.startAngle > arch.endAngle) {
                arch.endAngle += 360;
            }
            if (arch.endAngle - arch.startAngle > 360) {
                arch.endAngle -= 360;
            }
            pts_tmp = arch.controlPoints();
        } break;
        default: return;
        }
        break;
    }
    case GraphicsBasicItem::MouseRegion::DotRegion: pts_tmp[hoveredDotIndex()] += dp; break;
    default: return;
    }

    pointsChanged(pts_tmp);
}

void GraphicsArcItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    auto pts_tmp = geometryCache()->controlPoints();
    QPointF point = event->scenePos();
    if (pts_tmp.size() == 2 || pts_tmp.size() == 3) {
        pts_tmp.append(point);
        showHoverArc(pts_tmp);
    }
    if (!isValid()) {
        return;
    }
    GraphicsBasicItem::hoverMoveEvent(event);
    if (mouseRegion() == GraphicsBasicItem::MouseRegion::DotRegion) {
        return;
    }
    setMouseRegion(GraphicsBasicItem::MouseRegion::None);

    QPointF p1 = Utils::pointFromCenter(d_ptr->arch.center,
                                        d_ptr->arch.maxRadius,
                                        d_ptr->arch.startAngle);
    QPointF p2 = Utils::pointFromCenter(d_ptr->arch.center,
                                        d_ptr->arch.maxRadius,
                                        d_ptr->arch.endAngle);
    QLineF line1(p1, pts_tmp.at(0));
    QLineF line2(p2, pts_tmp.at(1));
    if (qAbs(Utils::distance(point, d_ptr->arch.center) - d_ptr->arch.minRadius) < margin() / 3) {
        d_ptr->mouseRegion = InEdge0;
        setMyCursor(d_ptr->arch.center, point);
    } else if (qAbs(Utils::distance(point, d_ptr->arch.center) - d_ptr->arch.maxRadius)
               < margin() / 3) {
        d_ptr->mouseRegion = InEdge1;
        setMyCursor(d_ptr->arch.center, point);
    } else if (Utils::boundingFromLine(line1, margin() / 4).containsPoint(point, Qt::OddEvenFill)) {
        d_ptr->mouseRegion = InEdgeL;
        setCursor(Utils::cursorForDirection(line1.angle()));
    } else if (Utils::boundingFromLine(line2, margin() / 4).containsPoint(point, Qt::OddEvenFill)) {
        d_ptr->mouseRegion = InEdgeH;
        setCursor(Utils::cursorForDirection(line2.angle()));
    } else if (d_ptr->arcPath.contains(point)) {
        setMouseRegion(GraphicsBasicItem::MouseRegion::All);
        setCursor(Qt::SizeAllCursor);
    } else {
        unsetCursor();
    }
}

void GraphicsArcItem::drawContent(QPainter *painter)
{
    painter->drawPath(isValid() ? d_ptr->arcPath : d_ptr->cachePath);
}

void GraphicsArcItem::pointsChanged(const QPolygonF &ply)
{
    auto rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }

    if (ply.size() == 3) {
        double deltaAngle = QLineF(ply[0], ply[1]).angleTo(QLineF(ply[0], ply[2]));
        if (deltaAngle < 0.00001 || deltaAngle > 355.99999) {
            return;
        }
    }

    switch (ply.size()) {
    case 1:
    case 2: geometryCache()->setControlPoints(ply); break;
    case 3: {
        if (!Utils::calculateHalfArc(ply, d_ptr->cachePath)) {
            return;
        }
        QPolygonF polygon = d_ptr->cachePath.toFillPolygon() + ply;
        if (!rect.contains(polygon.boundingRect())) {
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

void GraphicsArcItem::showHoverArc(const QPolygonF &ply)
{
    switch (ply.size()) {
    case 3:
        // QPainterPath::arcTo: Adding point with invalid coordinates, ignoring call
        if (Utils::distance(ply[1], ply[2]) < margin()) {
            return;
        }
        Utils::calculateHalfArc(ply, d_ptr->cachePath);
        break;
    case 4: Utils::calculateAllArc(ply, d_ptr->cachePath, margin()); break;
    default: return;
    }

    update();
}

} // namespace Graphics

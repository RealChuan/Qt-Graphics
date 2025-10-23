#include "graphicsringitem.h"
#include "geometrycache.hpp"
#include "graphicscircleitem.h"
#include "graphicsutils.hpp"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QtMath>

namespace Graphics {

auto Ring::boundingRect(double margin) const -> QRectF
{
    if (!isVaild(margin)) {
        return QRectF();
    }
    double addLen = margin / 2;
    QPointF point = QPointF(maxRadius, maxRadius);
    return QRectF(center - point, center + point).adjusted(-addLen, -addLen, addLen, addLen);
}

auto Ring::minBoundingRect(double margin) const -> QRectF
{
    if (!isVaild(margin)) {
        return QRectF();
    }
    double addLen = margin / 2;
    QPointF point = QPointF(minRadius, minRadius);
    return QRectF(center - point, center + point).adjusted(-addLen, -addLen, addLen, addLen);
}

auto Ring::isVaild(double margin) const -> bool
{
    return minRadius > 0 && maxRadius > minRadius && minRadius > margin
           && maxRadius - minRadius > margin;
}

auto Ring::controlPoints() const -> QPolygonF
{
    const double offset = minRadius * std::sqrt(2) / 2.0;

    return {
        center + QPointF(0, -maxRadius), // 上点
        center + QPointF(maxRadius, 0),  // 右点
        center + QPointF(0, maxRadius),  // 下点
        center + QPointF(-maxRadius, 0), // 左点

        // 内圆对角线控制点
        center + QPointF(offset, -offset),  // 右上点
        center + QPointF(-offset, -offset), // 左上点
        center + QPointF(-offset, offset),  // 左下点
        center + QPointF(offset, offset)    // 右下点
    };
}

class GraphicsRingItem::GraphicsRingItemPrivate
{
public:
    enum class MouseEdgeRegion : int { NoSelection, InnerEdge, OuterEdge };

    explicit GraphicsRingItemPrivate(GraphicsRingItem *q)
        : q_ptr(q)
    {}

    GraphicsRingItem *q_ptr;

    Ring ring;
    Circle circle;
    MouseEdgeRegion mouseRegion = MouseEdgeRegion::NoSelection;
};

GraphicsRingItem::GraphicsRingItem(QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsRingItemPrivate(this))
{}

GraphicsRingItem::GraphicsRingItem(const Ring &ring, QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsRingItemPrivate(this))
{
    setRing(ring);
}

GraphicsRingItem::~GraphicsRingItem() {}

auto GraphicsRingItem::setRing(const Ring &ring) -> bool
{
    if (!ring.isVaild(margin())) {
        return false;
    }
    auto rect = ring.boundingRect(margin());
    if (!scene()->sceneRect().contains(rect)) {
        return false;
    }
    rect = ring.boundingRect(0);
    QPainterPath shape;
    shape.addEllipse(rect);
    QPainterPath innerShape;
    innerShape.addEllipse(ring.minBoundingRect(0));
    shape -= innerShape;

    prepareGeometryChange();

    d_ptr->ring = ring;
    geometryCache()->setGeometryData(ring.controlPoints(), rect, shape);

    return true;
}

auto GraphicsRingItem::ring() const -> Ring
{
    return d_ptr->ring;
}

void GraphicsRingItem::drawContent(QPainter *painter)
{
    if (isValid()) {
        painter->drawEllipse(d_ptr->ring.boundingRect(0));
        painter->drawEllipse(d_ptr->ring.minBoundingRect(0));
    } else {
        switch (geometryCache()->controlPoints().size()) {
        case 1: painter->drawEllipse(d_ptr->circle.boundingRect(0)); break;
        case 2:
            painter->drawEllipse(d_ptr->ring.boundingRect(0));
            painter->drawEllipse(d_ptr->ring.minBoundingRect(0));
            break;
        default: break;
        }
    }
}

void GraphicsRingItem::pointsChanged(const QPolygonF &ply)
{
    auto sceneRect = scene()->sceneRect();

    switch (ply.size()) {
    case 1: geometryCache()->setControlPoints(ply); break;
    case 2: {
        Circle circle(ply[0], Utils::distance(ply[0], ply[1]));
        if (sceneRect.contains(circle.boundingRect(0)) && circle.isVaild(margin())) {
            d_ptr->circle = circle;
            geometryCache()->setControlPoints(ply);
        } else {
            return;
        }
    } break;
    case 3: {
        auto radius = Utils::distance(d_ptr->circle.center, ply[2]);
        auto ring = Ring{d_ptr->circle.center,
                         qMin(d_ptr->circle.radius, radius),
                         qMax(d_ptr->circle.radius, radius)};
        if (!setRing(ring)) {
            return;
        }
    } break;
    default: return;
    }
    update();
}

void GraphicsRingItem::updateHoverPreview(const QPointF &scenePos)
{
    auto controlPoints = geometryCache()->controlPoints();
    int size = controlPoints.size();
    if (size < 1 || size > 2) {
        return;
    }
    switch (size) {
    case 1: {
        Circle circle(controlPoints[0], Utils::distance(controlPoints[0], scenePos));
        if (!circle.isVaild(margin())) {
            return;
        }
        d_ptr->circle = circle;
    } break;
    case 2: {
        auto radius = Utils::distance(d_ptr->circle.center, scenePos);
        auto ring = Ring{d_ptr->circle.center,
                         qMin(radius, d_ptr->circle.radius),
                         qMax(radius, d_ptr->circle.radius)};
        if (!ring.isVaild(margin())) {
            return;
        }
        d_ptr->ring = ring;
    } break;
    default: return;
    }
    update();
}

GraphicsBasicItem::MouseRegion GraphicsRingItem::detectEdgeRegion(const QPointF &scenePos)
{
    const double edgeMargin = margin() / 2.0;
    const auto distance = Utils::distance(scenePos, d_ptr->ring.center);
    if (qAbs(distance - d_ptr->ring.minRadius) < edgeMargin) {
        d_ptr->mouseRegion = GraphicsRingItemPrivate::MouseEdgeRegion::InnerEdge;
        setMyCursor(d_ptr->ring.center, scenePos);
    } else if (qAbs(distance - d_ptr->ring.maxRadius) < edgeMargin) {
        d_ptr->mouseRegion = GraphicsRingItemPrivate::MouseEdgeRegion::OuterEdge;
        setMyCursor(d_ptr->ring.center, scenePos);
    } else {
        return GraphicsBasicItem::MouseRegion::NoSelection;
    }

    setMouseRegion(GraphicsBasicItem::MouseRegion::EdgeArea);
    return GraphicsBasicItem::MouseRegion::EdgeArea;
}

void GraphicsRingItem::handleMouseMoveEvent(const QPointF &scenePos,
                                            const QPointF &clickedPos,
                                            const QPointF delta)
{
    auto ring = d_ptr->ring;

    switch (mouseRegion()) {
    case GraphicsBasicItem::MouseRegion::EntireShape: ring.center += delta; break;
    case GraphicsBasicItem::MouseRegion::AnchorPoint:
        switch (hoveredDotIndex()) {
            // 外圆控制点 上下左右方向控制外圆半径
        case 0: // 上点 - 垂直移动控制外圆半径 向上拖动增加半径，向下拖动减小半径
            ring.maxRadius -= delta.y();
            break;
        case 1: // 右点 - 水平移动控制外圆半径 向右拖动增加半径，向左拖动减小半径
            ring.maxRadius += delta.x();
            break;
        case 2: // 下点 - 垂直移动控制外圆半径 向下拖动增加半径，向上拖动减小半径
            ring.maxRadius += delta.y();
            break;
        case 3: // 左点 - 水平移动控制外圆半径 向左拖动增加半径，向右拖动减小半径
            ring.maxRadius -= delta.x(); //
            break;
        // 内圆对角线控制点 沿对角线方向
        case 4: // 右上点 - 对角线移动控制内圆半径
            ring.minRadius += (delta.x() - delta.y()) / std::sqrt(2);
            break;
        case 5: // 左上点 - 对角线移动控制内圆半径
            ring.minRadius += (-delta.x() - delta.y()) / std::sqrt(2);
            break;
        case 6: // 左下点 - 对角线移动控制内圆半径
            ring.minRadius += (-delta.x() + delta.y()) / std::sqrt(2);
            break;
        case 7: // 右下点 - 对角线移动控制内圆半径
            ring.minRadius += (delta.x() + delta.y()) / std::sqrt(2);
            break;
        default: break;
        }
        break;
    case GraphicsBasicItem::MouseRegion::EdgeArea: {
        setMyCursor(ring.center, scenePos);
        auto distance = Utils::distance(ring.center, scenePos);
        switch (d_ptr->mouseRegion) {
        case GraphicsRingItemPrivate::MouseEdgeRegion::InnerEdge: ring.minRadius = distance; break;
        case GraphicsRingItemPrivate::MouseEdgeRegion::OuterEdge: ring.maxRadius = distance; break;
        default: break;
        }
    } break;
    default: break;
    }

    if (setRing(ring)) {
        update();
    }
}

} // namespace Graphics

#include "graphicscircleitem.h"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QtMath>

namespace Graphics {

auto Circle::isVaild(double margin) const -> bool
{
    return radius > 0 && radius > margin;
}

auto Circle::boundingRect(double margin) const -> QRectF
{
    if (!isVaild(margin)) {
        return QRectF();
    }
    return Utils::createBoundingRect(controlPoints(), margin / 2.0);
}

auto Circle::controlPoints() const -> QPolygonF
{
    return {
        center + QPointF(0, -radius), // 上点
        center + QPointF(radius, 0),  // 右点
        center + QPointF(0, radius),  // 下点
        center + QPointF(-radius, 0)  // 左点
    };
}

class GraphicsCircleItem::GraphicsCircleItemPrivate
{
public:
    explicit GraphicsCircleItemPrivate(GraphicsCircleItem *q)
        : q_ptr(q)
    {}

    GraphicsCircleItem *q_ptr;

    Circle circle;
};

GraphicsCircleItem::GraphicsCircleItem(QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsCircleItemPrivate(this))
{}

GraphicsCircleItem::GraphicsCircleItem(const Circle &cicrle, QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsCircleItemPrivate(this))
{
    setCircle(cicrle);
}

GraphicsCircleItem::~GraphicsCircleItem() {}

auto GraphicsCircleItem::setCircle(const Circle &circle) -> bool
{
    if (!circle.isVaild(margin())) {
        return false;
    }

    auto rect = circle.boundingRect(margin());
    if (!scene()->sceneRect().contains(rect)) {
        return false;
    }
    rect = circle.boundingRect(0);

    QPainterPath shape;
    shape.addEllipse(rect);

    prepareGeometryChange();
    d_ptr->circle = circle;
    geometryCache()->setGeometryData(d_ptr->circle.controlPoints(), rect, shape);

    return true;
}

auto GraphicsCircleItem::circle() const -> Circle
{
    return d_ptr->circle;
}

void GraphicsCircleItem::drawContent(QPainter *painter)
{
    painter->drawEllipse(d_ptr->circle.boundingRect(0));
}

void GraphicsCircleItem::pointsChanged(const QPolygonF &ply)
{
    switch (ply.size()) {
    case 1: geometryCache()->setControlPoints(ply); break;
    case 2:
        if (!setCircle({ply.first(), Utils::distance(ply.first(), ply.last())})) {
            return;
        }
        break;
    default: return;
    }

    update();
}

void GraphicsCircleItem::updateHoverPreview(const QPointF &scenePos)
{
    auto controlPoints = geometryCache()->controlPoints();
    if (controlPoints.size() != 1) {
        return;
    }

    Circle circle(controlPoints[0], Utils::distance(controlPoints[0], scenePos));
    if (!circle.isVaild(margin())) {
        return;
    }
    d_ptr->circle = circle;
    update();
}

GraphicsBasicItem::MouseRegion GraphicsCircleItem::detectEdgeRegion(const QPointF &scenePos)
{
    const auto &circle = d_ptr->circle;
    const double edgeMargin = margin() / 2.0;
    const double distance = Utils::distance(scenePos, circle.center);

    if (distance >= circle.radius - edgeMargin && distance <= circle.radius + edgeMargin) {
        setMouseRegion(MouseRegion::EdgeArea);
        setMyCursor(circle.center, scenePos);
        return MouseRegion::EdgeArea;
    }

    return MouseRegion::NoSelection;
}

void GraphicsCircleItem::handleMouseMoveEvent(const QPointF &scenePos,
                                              const QPointF &clickedPos,
                                              const QPointF delta)
{
    double radius = d_ptr->circle.radius;
    auto center = d_ptr->circle.center;

    switch (mouseRegion()) {
    case MouseRegion::EntireShape: center += delta; break;
    case MouseRegion::AnchorPoint:
        switch (hoveredDotIndex()) {
        case 0: // 上点 - 垂直移动控制半径 向上拖动增加半径，向下拖动减小半径
            radius -= delta.y();
            break;
        case 1: // 右点 - 水平移动控制半径 向右拖动增加半径，向左拖动减小半径
            radius += delta.x();
            break;
        case 2: // 下点 - 垂直移动控制半径 向下拖动增加半径，向上拖动减小半径
            radius += delta.y();
            break;
        case 3: // 左点 - 水平移动控制半径 向左拖动增加半径，向右拖动减小半径
            radius -= delta.x();
            break;
        default: break;
        }
        break;
    case MouseRegion::EdgeArea:
        setMyCursor(center, scenePos);
        radius = Utils::distance(center, scenePos);
        break;
    default: return;
    }

    if (setCircle({center, radius})) {
        update();
    }
}

} // namespace Graphics

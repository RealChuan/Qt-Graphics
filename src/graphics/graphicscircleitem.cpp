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
    double addLen = margin / 2;
    return Utils::createBoundingRect(controlPoints(), margin);
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
    Circle tempCircle;
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
    geometryCache()->setControlPoints(d_ptr->circle.controlPoints(), rect, shape);

    return true;
}

auto GraphicsCircleItem::circle() const -> Circle
{
    return d_ptr->circle;
}

auto GraphicsCircleItem::type() const -> int
{
    return Shape::CIRCLE;
}

void GraphicsCircleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == 0 || !isValid()) {
        return;
    }
    if (!isSelected()) {
        setSelected(true);
    }
    QPointF point = event->scenePos();
    QPointF dp = point - clickedPos();
    setClickedPos(point);

    double radius = d_ptr->circle.radius;
    QPointF center = d_ptr->circle.center;
    switch (mouseRegion()) {
    case MouseRegion::DotRegion:
        switch (hoveredDotIndex()) {
        case 0: // 上点 - 垂直移动控制半径 向上拖动增加半径，向下拖动减小半径
            radius -= dp.y();
            break;
        case 1: // 右点 - 水平移动控制半径 向右拖动增加半径，向左拖动减小半径
            radius += dp.x();
            break;
        case 2: // 下点 - 垂直移动控制半径 向下拖动增加半径，向上拖动减小半径
            radius += dp.y();
            break;
        case 3: // 左点 - 水平移动控制半径 向左拖动增加半径，向右拖动减小半径
            radius -= dp.x();
            break;
        default: break;
        }
        break;
    case MouseRegion::Edge: {
        QLineF lineF = QLineF(d_ptr->circle.center, point);
        setMyCursor(d_ptr->circle.center, point);
        radius = lineF.length();
    } break;
    case MouseRegion::All: center += dp; break;
    default: return;
    }

    if (setCircle({center, radius})) {
        update();
    }
}

void GraphicsCircleItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    auto scenePos = event->scenePos();
    auto pts_tmp = geometryCache()->controlPoints();
    if (pts_tmp.size() == 1) {
        pts_tmp.append(scenePos);
        showHoverCircle(pts_tmp);
    }

    GraphicsBasicItem::hoverMoveEvent(event);
    if (mouseRegion() == MouseRegion::DotRegion) {
        return;
    }

    const qreal distanceToCenter = Utils::distance(scenePos, d_ptr->circle.center);
    const qreal distanceToEdge = qAbs(distanceToCenter - d_ptr->circle.radius);
    if (distanceToEdge < margin() / 1.2) {
        setMouseRegion(MouseRegion::Edge);
        setMyCursor(d_ptr->circle.center, scenePos);
    }
}

void GraphicsCircleItem::drawContent(QPainter *painter)
{
    painter->drawEllipse(isValid() ? d_ptr->circle.boundingRect(0)
                                   : d_ptr->tempCircle.boundingRect(0));
}

void GraphicsCircleItem::pointsChanged(const QPolygonF &ply)
{
    auto rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }

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

void GraphicsCircleItem::showHoverCircle(const QPolygonF &ply)
{
    if (ply.size() != 2) {
        return;
    }
    Circle circle(ply.first(), Utils::distance(ply.first(), ply.last()));
    if (!circle.isVaild(margin())) {
        return;
    }
    d_ptr->tempCircle = circle;
    update();
}

} // namespace Graphics

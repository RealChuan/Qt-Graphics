#include "graphicscircleitem.h"
#include "graphics.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <QDebug>

namespace Graphics {

bool Circle::isVaild() const
{
    return qAbs(radius) > 0;
}

QRectF Circle::boundingRect() const
{
    if (!isVaild()) {
        return QRectF();
    }
    return QRectF(center - QPointF(radius, radius),
                  center + QPointF(radius, radius));
}

/*
 *
 */

struct GraphicsCircleItem::GraphicsCircleItemPrivate
{
    Circle circle;
    Circle tempCircle;
    QPainterPath shape;
};

GraphicsCircleItem::GraphicsCircleItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsCircleItemPrivate){}

GraphicsCircleItem::GraphicsCircleItem(const Circle &cicrle,
                                       QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsCircleItemPrivate)
{
    setCircle(cicrle);
}

GraphicsCircleItem::~GraphicsCircleItem(){}

bool GraphicsCircleItem::checkCircle(const Circle &circle, const double margin)
{
    return qAbs(circle.radius) > margin;
}

void computeCache(const Circle &circle, QPolygonF &cache)
{
    cache.clear();
    for(int i = 0; i <= 270; i += 90){
        double x = circle.radius * qCos(i * M_PI * 1.0 / 180.0) + circle.center.x();
        double y = circle.radius * qSin(i * M_PI * 1.0 / 180.0) + circle.center.y();
        cache.append(QPointF(x, y));
    }
}

void GraphicsCircleItem::setCircle(const Circle &circle)
{
    if (!checkCircle(circle, margin())) {
        return;
    }
    d_ptr->circle = circle;
    QPolygonF pts;
    computeCache(d_ptr->circle, pts);
    setCache(pts);

    QRectF rect = d_ptr->circle.boundingRect();
    rect.adjust(-margin(), -margin(), margin(), margin());
    d_ptr->shape.clear();
    d_ptr->shape.addEllipse(rect);
}

Circle GraphicsCircleItem::circle() const
{
    return d_ptr->circle;
}

bool GraphicsCircleItem::isValid() const
{
    return checkCircle(d_ptr->circle, margin());
}

int GraphicsCircleItem::type() const
{
    return CIRCLE;
}

QPainterPath GraphicsCircleItem::shape() const
{
    if (isValid()) {
        return d_ptr->shape;
    }
    return BasicGraphicsItem::shape();
}

void GraphicsCircleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    setClickedPos(event->scenePos());
    if (isValid()) {
        return;
    }
    QPointF point = event->pos();
    QPolygonF pts_tmp = cache();
    pts_tmp.append(point);
    pointsChanged(pts_tmp);
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
    case DotRegion:
        switch (hoveredDotIndex()) {
        case 0: radius += dp.x(); break;
        case 1: radius += dp.y(); break;
        case 2: radius -= dp.x(); break;
        case 3: radius -= dp.y(); break;
        default: break;
        }
        break;
    case Edge:{
        QLineF lineF = QLineF(d_ptr->circle.center, point);
        setMyCursor(d_ptr->circle.center, point);
        radius = lineF.length();
    } break;
    case All: center += dp; break;
    default: return;
    }

    Circle circle{center, radius};
    if (checkCircle(circle, margin()) && scene()->sceneRect().contains(circle.boundingRect())) {
        setCircle(circle);
        update();
    }
}

void GraphicsCircleItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF point = event->scenePos();
    QPolygonF pts_tmp = cache();
    if(pts_tmp.size() == 2){
        pts_tmp.append(point);
        showHoverCircle(pts_tmp);
    }

    if (!isValid()) {
        return;
    }
    BasicGraphicsItem::hoverMoveEvent(event);
    if (mouseRegion() == DotRegion) {
        return;
    }
    if (qAbs(Graphics::distance(point, d_ptr->circle.center) - d_ptr->circle.radius)
        < margin() / 3) {
        setMouseRegion(Edge);
        setMyCursor(d_ptr->circle.center, point);
    }
}

void GraphicsCircleItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    if (isValid()) {
        painter->drawEllipse(d_ptr->circle.boundingRect());
    } else {
        painter->drawEllipse(d_ptr->tempCircle.boundingRect());
    }
    if ((option->state & QStyle::State_Selected)) {
        drawAnchor(painter);
    }
}

void GraphicsCircleItem::pointsChanged(const QPolygonF &ply)
{
    QRectF rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }

    switch (ply.size()) {
    case 1:
    case 2:
        setCache(ply);
        break;
    case 3:{
        Circle circle;
        Graphics::calculateCircle(ply, circle.center, circle.radius);
        if (rect.contains(circle.boundingRect()) && checkCircle(circle, margin())) {
            setCircle(circle);
        } else {
            return;
        }
    } break;
    default: return;
    }

    update();
}

void GraphicsCircleItem::showHoverCircle(const QPolygonF &ply)
{
    if (ply.size() != 3) {
        return;
    }
    Graphics::calculateCircle(ply, d_ptr->tempCircle.center, d_ptr->tempCircle.radius);
    update();
}

}

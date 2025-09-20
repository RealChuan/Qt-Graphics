#include "graphicspolygonitem.h"
#include "graphics.h"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace Graphics {

#define PolygonMinPointSize 3

struct GraphicsPolygonItem::GraphicsPolygonItemPrivate
{
    QPolygonF polygon;
    QPolygonF tempPolygon;
};

GraphicsPolygonItem::GraphicsPolygonItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsPolygonItemPrivate)
{}

GraphicsPolygonItem::GraphicsPolygonItem(const QPolygonF &polygon, QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsPolygonItemPrivate)
{
    setPolygon(polygon);
}

GraphicsPolygonItem::~GraphicsPolygonItem() {}

inline auto checkPolygon(const QPolygonF &ply, const double margin) -> bool
{
    auto rect = ply.boundingRect();
    return ply.size() >= PolygonMinPointSize && rect.width() > margin && rect.height() > margin;
}

void GraphicsPolygonItem::setPolygon(const QPolygonF &ply)
{
    if (!checkPolygon(ply, margin())) {
        return;
    }
    prepareGeometryChange();
    d_ptr->polygon = ply;
    setCache(ply);
}

auto GraphicsPolygonItem::polygon() const -> QPolygonF
{
    return d_ptr->polygon;
}

auto GraphicsPolygonItem::isValid() const -> bool
{
    return checkPolygon(d_ptr->polygon, margin());
}

auto GraphicsPolygonItem::type() const -> int
{
    return POLYGON;
}

void GraphicsPolygonItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    setClickedPos(event->scenePos());
    if (isValid()) {
        return;
    }
    QPolygonF pts_tmp = cache();
    pts_tmp.append(event->pos());
    pointsChanged(pts_tmp);
}

void GraphicsPolygonItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == 0 || !isValid()) {
        return;
    }
    if (!isSelected()) {
        setSelected(true);
    }
    QPointF point = event->scenePos();
    QPolygonF pts_tmp = cache();

    switch (mouseRegion()) {
    case DotRegion: pts_tmp.replace(hoveredDotIndex(), point); break;
    case All: {
        QPointF dp = point - clickedPos();
        setClickedPos(point);
        pts_tmp.translate(dp);
    } break;
    default: return;
    }

    if (scene()->sceneRect().contains(pts_tmp.boundingRect()) && checkPolygon(pts_tmp, margin())) {
        setPolygon(pts_tmp);
        update();
    }
}

void GraphicsPolygonItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPolygonF pts_tmp = cache();
    if (pts_tmp.size() > 0) {
        pts_tmp.append(event->scenePos());
        showHoverPolygon(pts_tmp);
    }

    BasicGraphicsItem::hoverMoveEvent(event);
}

void GraphicsPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    if (isValid()) {
        painter->drawPolygon(d_ptr->polygon);
    } else {
        painter->drawPolyline(d_ptr->tempPolygon);
    }
    if (option->state & QStyle::State_Selected) {
        drawAnchor(painter);
        drawBoundingRect(painter);
    }
}

void GraphicsPolygonItem::pointsChanged(const QPolygonF &ply)
{
    QRectF rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }
    int size = ply.size();
    if (size < 3) {
        setCache(ply);
    } else {
        if (Graphics::distance(ply.first(), ply.last()) < margin() && checkPolygon(ply, margin())) {
            QPolygonF pts_tmp = ply;
            pts_tmp.removeLast();
            setPolygon(pts_tmp);
        } else {
            setCache(ply);
        }
    }
    update();
}

void GraphicsPolygonItem::showHoverPolygon(const QPolygonF &ply)
{
    d_ptr->tempPolygon = ply;
    update();
}

} // namespace Graphics

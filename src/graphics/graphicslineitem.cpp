#include "graphicslineitem.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace Graphics {

class GraphicsLineItem::GraphicsLineItemPrivate
{
public:
    explicit GraphicsLineItemPrivate(GraphicsLineItem *q)
        : q_ptr(q)
    {}

    GraphicsLineItem *q_ptr;

    QLineF line;
    QLineF tempLine;
};

GraphicsLineItem::GraphicsLineItem(QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsLineItemPrivate(this))
{}

GraphicsLineItem::GraphicsLineItem(const QLineF &line, QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsLineItemPrivate(this))
{
    setLine(line);
}

GraphicsLineItem::~GraphicsLineItem() {}

inline auto checkLine(const QLineF &line, const double margin) -> bool
{
    return !line.isNull() && line.length() > margin;
}

void GraphicsLineItem::setLine(const QLineF &line)
{
    if (!checkLine(line, margin())) {
        return;
    }
    prepareGeometryChange();
    d_ptr->line = line;
    QPolygonF cache;
    cache << line.p1() << line.p2();
    setCache(cache);
    emit lineChnaged(line);
}

auto GraphicsLineItem::line() const -> QLineF
{
    return d_ptr->line;
}

auto GraphicsLineItem::isValid() const -> bool
{
    return checkLine(d_ptr->line, margin());
}

auto GraphicsLineItem::type() const -> int
{
    return LINE;
}

void GraphicsLineItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return GraphicsBasicItem::mousePressEvent(event);
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

void GraphicsLineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == 0 || !isValid()) {
        return;
    }
    if (!isSelected()) {
        setSelected(true);
    }
    QPointF point = event->scenePos();
    QPolygonF pts_tmp = cache();
    QPointF dp = point - clickedPos();
    setClickedPos(point);

    switch (mouseRegion()) {
    case DotRegion: pts_tmp.replace(hoveredDotIndex(), point); break;
    case All: pts_tmp.translate(dp); break;
    default: return;
    }
    pointsChanged(pts_tmp);
}

void GraphicsLineItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPolygonF pts_tmp = cache();
    if (pts_tmp.size() == 1) {
        pts_tmp.append(event->scenePos());
        showHoverLine(pts_tmp);
    }

    GraphicsBasicItem::hoverMoveEvent(event);
}

void GraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    if (isValid()) {
        painter->drawLine(d_ptr->line);
    } else {
        painter->drawLine(d_ptr->tempLine);
    }
    if (option->state & QStyle::State_Selected) {
        drawAnchor(painter);
        drawBoundingRect(painter);
    }
}

void GraphicsLineItem::pointsChanged(const QPolygonF &ply)
{
    QRectF rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }
    switch (ply.size()) {
    case 1: setCache(ply); break;
    case 2: {
        QLineF line(ply[0], ply[1]);
        if (checkLine(line, margin())) {
            setLine(line);
        } else {
            return;
        }
    } break;
    default: return;
    }
    update();
}

void GraphicsLineItem::showHoverLine(const QPolygonF &ply)
{
    if (ply.size() != 2) {
        return;
    }
    d_ptr->tempLine.setPoints(ply[0], ply[1]);
    update();
}

} // namespace Graphics

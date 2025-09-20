#include "graphicsroundedrectitem.hpp"
#include "graphics.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace Graphics {

inline auto checkRect(const QRectF &rect, const double margin) -> bool
{
    return rect.isValid() && rect.x() >= 0 && rect.y() >= 0 && rect.width() > margin
           && rect.height() > margin;
}

RoundedRect::RoundedRect(const QRectF &rect, qreal xRadius, qreal yRadius)
{
    this->rect = rect;
    this->xRadius = xRadius;
    this->yRadius = yRadius;
}

bool RoundedRect::isValid() const
{
    return rect.isValid() && xRadius >= 0 && yRadius >= 0 && xRadius < rect.width() / 2
           && yRadius < rect.height() / 2;
}

class GraphicsRoundedRectItem::GraphicsRoundedRectItemPrivate
{
public:
    explicit GraphicsRoundedRectItemPrivate(GraphicsRoundedRectItem *q)
        : q_ptr(q)
    {
        qRegisterMetaType<Graphics::RoundedRect>("Graphics::RoundedRect");
    }

    GraphicsRoundedRectItem *q_ptr = nullptr;

    bool linehovered = false;
    QLineF hoveredLine;
};

GraphicsRoundedRectItem::GraphicsRoundedRectItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsRoundedRectItemPrivate(this))
{}

GraphicsRoundedRectItem::GraphicsRoundedRectItem(const RoundedRect &roundedRect,
                                                 QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsRoundedRectItemPrivate(this))
{
    setRoundedRect(roundedRect);
}

GraphicsRoundedRectItem::~GraphicsRoundedRectItem() {}

void GraphicsRoundedRectItem::setRoundedRect(const RoundedRect &roundedRect)
{
    if (!roundedRect.isValid() || !checkRect(roundedRect.rect, margin())) {
        return;
    }
    prepareGeometryChange();
    m_roundedRect = roundedRect;
    QPolygonF cache;
    cache << roundedRect.rect.topLeft() << roundedRect.rect.bottomRight();
    setCache(cache);
    emit roundedRectChanged(m_roundedRect);
}

RoundedRect GraphicsRoundedRectItem::roundedRect() const
{
    return m_roundedRect;
}

auto GraphicsRoundedRectItem::isValid() const -> bool
{
    return m_roundedRect.isValid() && checkRect(m_roundedRect.rect, margin());
}

auto GraphicsRoundedRectItem::type() const -> int
{
    return ROUNDEDRECT;
}

void GraphicsRoundedRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return BasicGraphicsItem::mousePressEvent(event);
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

auto polygonFromRect(const QRectF &rect) -> QPolygonF
{
    QPolygonF ply;
    ply.append(rect.topLeft());
    ply.append(rect.topRight());
    ply.append(rect.bottomRight());
    ply.append(rect.bottomLeft());
    return ply;
}

void GraphicsRoundedRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
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

    if (d_ptr->linehovered) {
        QPointF p1 = d_ptr->hoveredLine.p1();
        QPointF p2 = d_ptr->hoveredLine.p2();

        QPolygonF ply1 = polygonFromRect(m_roundedRect.rect);
        int index0 = ply1.indexOf(p1);
        int index1 = ply1.indexOf(p2);
        if (index0 < 0 || index1 < 0) {
            return;
        }
        if (abs(p1.x() - p2.x()) < 0.0001) { //vertical line
            p1 = p1 + QPointF(dp.x(), 0);
            p2 = p2 + QPointF(dp.x(), 0);
        } else {
            p1 = p1 + QPointF(0, dp.y());
            p2 = p2 + QPointF(0, dp.y());
        }

        ply1.replace(index0, p1);
        ply1.replace(index1, p2);

        d_ptr->hoveredLine = QLineF(p1, p2);

        pts_tmp.clear();
        pts_tmp.append(ply1.at(0));
        pts_tmp.append(ply1.at(2));
    } else {
        switch (mouseRegion()) {
        case DotRegion: {
            int index = hoveredDotIndex();
            pts_tmp.replace(index, point);
        } break;
        case All: pts_tmp.translate(dp); break;
        default: return;
        }
    }
    pointsChanged(pts_tmp);
}

void GraphicsRoundedRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF point = event->scenePos();
    QPolygonF pts_tmp = cache();
    if (pts_tmp.size() == 1) {
        pts_tmp.append(point);
        showHoverRect(pts_tmp);
    }
    if (!isValid()) {
        return;
    }
    BasicGraphicsItem::hoverMoveEvent(event);
    if (mouseRegion() == DotRegion) {
        return;
    }
    QPolygonF ply = polygonFromRect(m_roundedRect.rect);
    for (int i = 0; i < ply.count(); ++i) {
        QLineF pl(ply.at(i), ply.at((i + 1) % 4));
        QPolygonF tmp = Graphics::boundingFromLine(pl, margin() / 4);
        if (tmp.containsPoint(point, Qt::OddEvenFill)) {
            d_ptr->linehovered = true;
            d_ptr->hoveredLine = pl;
            setCursor(Graphics::curorFromAngle(pl.angle()));
            return;
        }
    }

    d_ptr->linehovered = false;
}

void GraphicsRoundedRectItem::paint(QPainter *painter,
                                    const QStyleOptionGraphicsItem *option,
                                    QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());
    auto roundedRect = isValid() ? m_roundedRect : m_tempRoundedRect;
    painter->drawRoundedRect(roundedRect.rect, roundedRect.xRadius, roundedRect.yRadius);

    if (option->state & QStyle::State_Selected) {
        drawAnchor(painter);
        drawBoundingRect(painter);
    }
}

void GraphicsRoundedRectItem::pointsChanged(const QPolygonF &ply)
{
    QRectF rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }
    switch (ply.size()) {
    case 1: setCache(ply); break;
    case 2: {
        QRectF rect_(ply[0], ply[1]);
        if (checkRect(rect_, margin())) {
            setRoundedRect({rect_, m_roundedRect.xRadius, m_roundedRect.yRadius});
        } else {
            return;
        }
    } break;
    default: return;
    }
    update();
}

void GraphicsRoundedRectItem::showHoverRect(const QPolygonF &ply)
{
    if (ply.size() != 2) {
        return;
    }
    m_tempRoundedRect.rect = QRectF(ply[0], ply[1]);
    update();
}

} // namespace Graphics

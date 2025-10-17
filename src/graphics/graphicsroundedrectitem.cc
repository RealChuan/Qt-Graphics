#include "graphicsroundedrectitem.hpp"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

namespace Graphics {

RoundedRect::RoundedRect(const QRectF &rect, qreal xRadius, qreal yRadius)
{
    this->rect = rect;
    this->xRadius = xRadius;
    this->yRadius = yRadius;
}

bool RoundedRect::isValid(double margin) const
{
    return rect.isValid() && xRadius >= 0 && yRadius >= 0 && xRadius < rect.width() / 2
           && yRadius < rect.height() / 2 && rect.x() >= 0 && rect.y() >= 0 && rect.width() > margin
           && rect.height() > margin;
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

    QLineF hoveredLine;
};

GraphicsRoundedRectItem::GraphicsRoundedRectItem(QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsRoundedRectItemPrivate(this))
{}

GraphicsRoundedRectItem::GraphicsRoundedRectItem(const RoundedRect &roundedRect,
                                                 QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsRoundedRectItemPrivate(this))
{
    setRoundedRect(roundedRect);
}

GraphicsRoundedRectItem::~GraphicsRoundedRectItem() {}

auto GraphicsRoundedRectItem::setRoundedRect(const RoundedRect &roundedRect) -> bool
{
    if (!roundedRect.isValid(margin())) {
        return false;
    }

    QPolygonF anchorPoints{roundedRect.rect.topLeft(), roundedRect.rect.bottomRight()};
    auto rect = Utils::createBoundingRect(anchorPoints, margin());
    if (!scene()->sceneRect().contains(rect)) {
        return false;
    }
    QPainterPath shape;
    shape.addRoundedRect(roundedRect.rect, roundedRect.xRadius, roundedRect.yRadius);

    prepareGeometryChange();
    m_roundedRect = roundedRect;
    geometryCache()->setControlPoints(anchorPoints,
                                      Utils::createBoundingRect(anchorPoints, 0),
                                      shape);
    emit roundedRectChanged(m_roundedRect);

    return true;
}

RoundedRect GraphicsRoundedRectItem::roundedRect() const
{
    return m_roundedRect;
}

auto GraphicsRoundedRectItem::type() const -> int
{
    return Shape::ROUNDEDRECT;
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
    auto pts_tmp = geometryCache()->controlPoints();
    QPointF dp = point - clickedPos();
    setClickedPos(point);

    switch (mouseRegion()) {
    case MouseRegion::DotRegion: {
        int index = hoveredDotIndex();
        pts_tmp.replace(index, point);
    } break;
    case MouseRegion::Edge: {
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
    } break;
    case MouseRegion::All: pts_tmp.translate(dp); break;
    default: return;
    }

    pointsChanged(pts_tmp);
}

void GraphicsRoundedRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF point = event->scenePos();
    auto pts_tmp = geometryCache()->controlPoints();
    if (pts_tmp.size() == 1) {
        pts_tmp.append(point);
        showHoverRect(pts_tmp);
    }
    if (!isValid()) {
        return;
    }

    GraphicsBasicItem::hoverMoveEvent(event);
    if (mouseRegion() == MouseRegion::DotRegion) {
        return;
    }
    QPolygonF ply = polygonFromRect(m_roundedRect.rect);
    for (int i = 0; i < ply.count(); ++i) {
        QLineF pl(ply.at(i), ply.at((i + 1) % 4));
        QPolygonF tmp = Utils::boundingFromLine(pl, margin() / 4);
        if (tmp.containsPoint(point, Qt::OddEvenFill)) {
            setMouseRegion(MouseRegion::Edge);
            d_ptr->hoveredLine = pl;
            setCursor(Utils::cursorForDirection(pl.angle()));
            return;
        }
    }
}

void GraphicsRoundedRectItem::drawContent(QPainter *painter)
{
    auto roundedRect = isValid() ? m_roundedRect : m_tempRoundedRect;
    painter->drawRoundedRect(roundedRect.rect, roundedRect.xRadius, roundedRect.yRadius);
}

void GraphicsRoundedRectItem::pointsChanged(const QPolygonF &ply)
{
    auto rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }

    switch (ply.size()) {
    case 1: geometryCache()->setControlPoints(ply); break;
    case 2:
        if (!setRoundedRect(RoundedRect(QRectF(ply[0], ply[1]).normalized(),
                                        m_roundedRect.xRadius,
                                        m_roundedRect.yRadius))) {
            return;
        }
        break;
    default: return;
    }
    update();
}

void GraphicsRoundedRectItem::showHoverRect(const QPolygonF &ply)
{
    if (ply.size() != 2) {
        return;
    }
    m_tempRoundedRect.rect = QRectF(ply[0], ply[1]).normalized();
    update();
}

} // namespace Graphics

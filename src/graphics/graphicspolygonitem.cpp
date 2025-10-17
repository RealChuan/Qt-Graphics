#include "graphicspolygonitem.h"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

namespace Graphics {

class GraphicsPolygonItem::GraphicsPolygonItemPrivate
{
public:
    explicit GraphicsPolygonItemPrivate(GraphicsPolygonItem *q)
        : q_ptr(q)
    {}

    GraphicsPolygonItem *q_ptr;

    QPolygonF polygon;
    QPolygonF tempPolygon;
};

GraphicsPolygonItem::GraphicsPolygonItem(QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsPolygonItemPrivate(this))
{}

GraphicsPolygonItem::GraphicsPolygonItem(const QPolygonF &polygon, QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsPolygonItemPrivate(this))
{
    setPolygon(polygon);
}

GraphicsPolygonItem::~GraphicsPolygonItem() {}

inline auto checkPolygonVaild(const QPolygonF &ply, const double margin) -> bool
{
    auto rect = ply.boundingRect();
    return ply.size() >= 3 && rect.width() > margin && rect.height() > margin;
}

auto GraphicsPolygonItem::setPolygon(const QPolygonF &ply) -> bool
{
    if (!checkPolygonVaild(ply, margin())) {
        return false;
    }

    auto rect = Utils::createBoundingRect(ply, margin());
    if (!scene()->sceneRect().contains(rect)) {
        return false;
    }

    prepareGeometryChange();

    d_ptr->polygon = ply;

    geometryCache()->setAnchorPoints(ply, Utils::createBoundingRect(ply, 0));

    return true;
}

auto GraphicsPolygonItem::polygon() const -> QPolygonF
{
    return d_ptr->polygon;
}

auto GraphicsPolygonItem::type() const -> int
{
    return Shape::POLYGON;
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
    auto pts_tmp = geometryCache()->anchorPoints();

    switch (mouseRegion()) {
    case MouseRegion::DotRegion: pts_tmp.replace(hoveredDotIndex(), point); break;
    case MouseRegion::All: {
        QPointF dp = point - clickedPos();
        setClickedPos(point);
        pts_tmp.translate(dp);
    } break;
    default: return;
    }
    if (setPolygon(pts_tmp)) {
        update();
    }
}

void GraphicsPolygonItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    auto pts_tmp = geometryCache()->anchorPoints();
    if (pts_tmp.size() > 0) {
        pts_tmp.append(event->scenePos());
        showHoverPolygon(pts_tmp);
    }

    GraphicsBasicItem::hoverMoveEvent(event);
}

void GraphicsPolygonItem::drawContent(QPainter *painter)
{
    if (isValid()) {
        painter->drawPolygon(d_ptr->polygon);
    } else {
        painter->drawPolyline(d_ptr->tempPolygon);
    }
}

void GraphicsPolygonItem::pointsChanged(const QPolygonF &ply)
{
    auto rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }

    if (ply.size() < 3) {
        geometryCache()->setAnchorPoints(ply, {});
    } else {
        if (Utils::distance(ply.first(), ply.last()) < margin()) {
            QPolygonF pts_tmp = ply;
            pts_tmp.removeLast();
            if (!setPolygon(pts_tmp)) {
                geometryCache()->setAnchorPoints(ply, {});
            }
        } else {
            geometryCache()->setAnchorPoints(ply, {});
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

#include "graphicspolygonitem.h"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

namespace Graphics {

namespace {

auto checkPolygonVaild(const QPolygonF &ply, const double margin) -> bool
{
    auto rect = ply.boundingRect();
    return ply.size() >= 3 && rect.width() > margin && rect.height() > margin;
}

QPainterPath simplifiedPath(const QPolygonF &ply)
{
    QPainterPath originalPath;
    originalPath.setFillRule(Qt::WindingFill);
    originalPath.addPolygon(ply);
    auto simplifiedPath = originalPath.simplified();
    simplifiedPath.setFillRule(Qt::WindingFill);
    return simplifiedPath; // 复杂自相交情况会出现空心，无法选中

    // QPainterPath path;
    // path.addPolygon(ply.boundingRect());
    // return path;
}

} // namespace

class GraphicsPolygonItem::GraphicsPolygonItemPrivate
{
public:
    explicit GraphicsPolygonItemPrivate(GraphicsPolygonItem *q)
        : q_ptr(q)
    {}

    GraphicsPolygonItem *q_ptr;

    QPolygonF polygon;
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
    geometryCache()->setControlPoints(ply, Utils::createBoundingRect(ply, 0), simplifiedPath(ply));

    return true;
}

auto GraphicsPolygonItem::polygon() const -> QPolygonF
{
    return d_ptr->polygon;
}

void GraphicsPolygonItem::drawContent(QPainter *painter)
{
    if (isValid()) {
        painter->drawPolygon(d_ptr->polygon);
    } else {
        painter->drawPolyline(d_ptr->polygon);
    }
}

void GraphicsPolygonItem::pointsChanged(const QPolygonF &ply)
{
    if (ply.size() < 3) {
        geometryCache()->setControlPoints(ply);
    } else if (!checkPolygonVaild(ply, margin())) {
        return;
    } else {
        if (Utils::distance(ply.first(), ply.last()) < margin()) {
            QPolygonF pts_tmp = ply;
            pts_tmp.removeLast();
            if (!setPolygon(pts_tmp)) {
                geometryCache()->setControlPoints(ply);
            }
        } else {
            geometryCache()->setControlPoints(ply);
        }
    }
    update();
}

void GraphicsPolygonItem::updateHoverPreview(const QPointF &scenePos)
{
    auto polygon = geometryCache()->controlPoints();
    polygon.append(scenePos);
    if (!checkPolygonVaild(polygon, margin())) {
        return;
    }

    d_ptr->polygon = polygon;
    update();
}

void GraphicsPolygonItem::handleMouseMoveEvent(const QPointF &scenePos,
                                               const QPointF &clickedPos,
                                               const QPointF delta)
{
    auto controlPoints = geometryCache()->controlPoints();

    switch (mouseRegion()) {
    case MouseRegion::EntireShape: controlPoints.translate(delta); break;
    case MouseRegion::AnchorPoint: controlPoints.replace(hoveredDotIndex(), scenePos); break;
    default: return;
    }
    if (setPolygon(controlPoints)) {
        update();
    }
}

} // namespace Graphics

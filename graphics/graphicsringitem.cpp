#include "graphicsringitem.h"
#include "graphics.h"
#include "graphicscircleitem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>

namespace Graphics {

#define CircleMaxSize 4

QRectF Ring::boundingRect() const
{
    if (!isVaild()) {
        return QRectF();
    }
    QPointF point = QPointF(maxRadius, maxRadius);
    return QRectF(center - point, center + point);
}

QRectF Ring::minBoundingRect() const
{
    if (!isVaild()) {
        return QRectF();
    }
    QPointF point = QPointF(minRadius, minRadius);
    return QRectF(center - point, center + point);
}

bool Ring::isVaild() const
{
    return minRadius > 0 && maxRadius > minRadius;
}

struct GraphicsRingItem::GraphicsRingItemPrivate
{
    Ring ring;
    Ring tempRing;
    Circle maxCircle;
    GraphicsRingItem::MouseRegion mouseRegion = GraphicsRingItem::None;
    QPainterPath shape;
};

GraphicsRingItem::GraphicsRingItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsRingItemPrivate)
{}

GraphicsRingItem::GraphicsRingItem(const Ring &ring, QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsRingItemPrivate)
{
    setRing(ring);
}

GraphicsRingItem::~GraphicsRingItem() {}

inline bool checkRing(const Ring &ring, const double margin)
{
    return ring.minRadius > margin && ring.maxRadius - ring.minRadius > margin;
}

inline void computeCache(const Ring &ring, QPolygonF &pts)
{
    pts.clear();
    for (int i = 0; i <= 270; i += 90) {
        double x = ring.maxRadius * qCos(i * M_PI * 1.0 / 180.0) + ring.center.x();
        double y = ring.maxRadius * qSin(i * M_PI * 1.0 / 180.0) + ring.center.y();
        pts.append(QPointF(x, y));
    }

    for (int i = 45; i <= 315; i += 90) {
        double x = ring.minRadius * qCos(i * M_PI * 1.0 / 180.0) + ring.center.x();
        double y = ring.minRadius * qSin(i * M_PI * 1.0 / 180.0) + ring.center.y();
        pts.append(QPointF(x, y));
    }
}

void GraphicsRingItem::setRing(const Ring &ring)
{
    if (!checkRing(ring, margin())) {
        return;
    }
    prepareGeometryChange();

    d_ptr->ring = ring;
    QPolygonF pts;
    computeCache(d_ptr->ring, pts);
    setCache(pts);

    QRectF rect = d_ptr->ring.boundingRect();
    rect.adjust(-margin(), -margin(), margin(), margin());
    d_ptr->shape.clear();
    d_ptr->shape.addEllipse(rect);
}

Ring GraphicsRingItem::ring() const
{
    return d_ptr->ring;
}

bool GraphicsRingItem::isValid() const
{
    return checkRing(d_ptr->ring, margin());
}

int GraphicsRingItem::type() const
{
    return RING;
}

QPainterPath GraphicsRingItem::shape() const
{
    if (isValid()) {
        return d_ptr->shape;
    }
    return BasicGraphicsItem::shape();
}

void GraphicsRingItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
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

void GraphicsRingItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
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

    Ring ring = d_ptr->ring;
    switch (mouseRegion()) {
    case BasicGraphicsItem::DotRegion:
        switch (hoveredDotIndex()) {
        case 0: ring.maxRadius += dp.x(); break;
        case 1: ring.maxRadius += dp.y(); break;
        case 2: ring.maxRadius -= dp.x(); break;
        case 3: ring.maxRadius -= dp.y(); break;
        case 4: ring.minRadius += dp.x(); break;
        case 5: ring.minRadius += dp.y(); break;
        case 6: ring.minRadius -= dp.x(); break;
        case 7: ring.minRadius -= dp.y(); break;
        default: break;
        }
        break;
    case BasicGraphicsItem::All: ring.center += dp; break;
    case BasicGraphicsItem::None:
        switch (d_ptr->mouseRegion) {
        case InEdge0:
            setMyCursor(ring.center, point);
            ring.minRadius = Graphics::distance(ring.center, point);
            break;
        case InEdge1:
            setMyCursor(ring.center, point);
            ring.maxRadius = Graphics::distance(ring.center, point);
            break;
        default: break;
        }
        break;
    default: break;
    }

    if (scene()->sceneRect().contains(ring.boundingRect()) && checkRing(ring, margin())) {
        setRing(ring);
        update();
    }
}

void GraphicsRingItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF point = event->scenePos();
    QPolygonF pts_tmp = cache();
    int size = pts_tmp.size();
    if (size == 2 || size == 3) {
        pts_tmp.append(point);
        showHoverRing(pts_tmp);
    }
    if (!isValid()) {
        return;
    }
    BasicGraphicsItem::hoverMoveEvent(event);
    if (mouseRegion() == DotRegion) {
        return;
    }
    setMouseRegion(BasicGraphicsItem::None);

    if (qAbs(Graphics::distance(point, d_ptr->ring.center) - d_ptr->ring.minRadius) < margin() / 3) {
        d_ptr->mouseRegion = InEdge0;
        setMyCursor(d_ptr->ring.center, point);
    } else if (qAbs(Graphics::distance(point, d_ptr->ring.center) - d_ptr->ring.maxRadius)
               < margin() / 3) {
        d_ptr->mouseRegion = InEdge1;
        setMyCursor(d_ptr->ring.center, point);
    } else if (shape().contains(point)) {
        setMouseRegion(BasicGraphicsItem::All);
        setCursor(Qt::SizeAllCursor);
    } else {
        unsetCursor();
    }
}

void GraphicsRingItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    if (isValid()) {
        painter->drawEllipse(d_ptr->ring.boundingRect());
        painter->drawEllipse(d_ptr->ring.minBoundingRect());
    } else {
        switch (cache().size()) {
        case 2: painter->drawEllipse(d_ptr->maxCircle.boundingRect()); break;
        case 3:
            painter->drawEllipse(d_ptr->tempRing.boundingRect());
            painter->drawEllipse(d_ptr->tempRing.minBoundingRect());
            break;
        default: break;
        }
    }

    if (option->state & QStyle::State_Selected) {
        drawAnchor(painter);
    }
}

void GraphicsRingItem::pointsChanged(const QPolygonF &ply)
{
    QRectF rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }
    switch (ply.size()) {
    case 1:
    case 2: setCache(ply); break;
    case 3: { //外圈
        Graphics::calculateCircle(ply, d_ptr->maxCircle.center, d_ptr->maxCircle.radius);
        if (rect.contains(d_ptr->maxCircle.boundingRect())
            && GraphicsCircleItem::checkCircle(d_ptr->maxCircle, margin())) {
            setCache(ply);
        } else {
            return;
        }
    } break;
    case 4: {
        double minRadius = Graphics::distance(d_ptr->maxCircle.center, ply[3]);
        Ring ring{d_ptr->maxCircle.center, minRadius, d_ptr->maxCircle.radius};
        if (checkRing(ring, margin())) {
            setRing(ring);
        } else {
            return;
        }
    } break;
    default: return;
    }
    update();
}

void GraphicsRingItem::showHoverRing(const QPolygonF &ply)
{
    switch (ply.size()) {
    case 3: Graphics::calculateCircle(ply, d_ptr->maxCircle.center, d_ptr->maxCircle.radius); break;
    case 4: {
        double minRadius = Graphics::distance(d_ptr->maxCircle.center, ply[3]);
        if (minRadius >= d_ptr->maxCircle.radius) {
            return;
        }
        d_ptr->tempRing = Ring{d_ptr->maxCircle.center, minRadius, d_ptr->maxCircle.radius};
    } break;
    default: return;
    }
    update();
}

} // namespace Graphics

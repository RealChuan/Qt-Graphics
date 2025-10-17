#include "graphicsringitem.h"
#include "geometrycache.hpp"
#include "graphicscircleitem.h"
#include "graphicsutils.hpp"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QtMath>

namespace Graphics {

auto Ring::boundingRect(double margin) const -> QRectF
{
    if (!isVaild(margin)) {
        return QRectF();
    }
    double addLen = margin / 2;
    QPointF point = QPointF(maxRadius, maxRadius);
    return QRectF(center - point, center + point).adjusted(-addLen, -addLen, addLen, addLen);
}

auto Ring::minBoundingRect(double margin) const -> QRectF
{
    if (!isVaild(margin)) {
        return QRectF();
    }
    double addLen = margin / 2;
    QPointF point = QPointF(minRadius, minRadius);
    return QRectF(center - point, center + point).adjusted(-addLen, -addLen, addLen, addLen);
}

auto Ring::isVaild(double margin) const -> bool
{
    return minRadius > 0 && maxRadius > minRadius && minRadius > margin
           && maxRadius - minRadius > margin;
}

auto Ring::controlPoints() const -> QPolygonF
{
    const double offset = minRadius * std::sqrt(2) / 2.0;

    return {
        center + QPointF(0, -maxRadius), // 上点
        center + QPointF(maxRadius, 0),  // 右点
        center + QPointF(0, maxRadius),  // 下点
        center + QPointF(-maxRadius, 0), // 左点

        // 内圆对角线控制点
        center + QPointF(offset, -offset),  // 右上点
        center + QPointF(-offset, -offset), // 左上点
        center + QPointF(-offset, offset),  // 左下点
        center + QPointF(offset, offset)    // 右下点
    };
}

class GraphicsRingItem::GraphicsRingItemPrivate
{
public:
    explicit GraphicsRingItemPrivate(GraphicsRingItem *q)
        : q_ptr(q)
    {}

    GraphicsRingItem *q_ptr;

    Ring ring;
    Ring tempRing;
    Circle maxCircle;
    GraphicsRingItem::MouseRegion mouseRegion = GraphicsRingItem::None;
};

GraphicsRingItem::GraphicsRingItem(QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsRingItemPrivate(this))
{}

GraphicsRingItem::GraphicsRingItem(const Ring &ring, QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsRingItemPrivate(this))
{
    setRing(ring);
}

GraphicsRingItem::~GraphicsRingItem() {}

auto GraphicsRingItem::setRing(const Ring &ring) -> bool
{
    if (!ring.isVaild(margin())) {
        return false;
    }
    auto rect = ring.boundingRect(margin());
    if (!scene()->sceneRect().contains(rect)) {
        return false;
    }
    rect = ring.boundingRect(0);
    QPainterPath shape;
    shape.addEllipse(rect);
    QPainterPath innerShape;
    innerShape.addEllipse(ring.minBoundingRect(0));
    shape -= innerShape;

    prepareGeometryChange();

    d_ptr->ring = ring;
    geometryCache()->setControlPoints(ring.controlPoints(), rect, shape);

    return true;
}

auto GraphicsRingItem::ring() const -> Ring
{
    return d_ptr->ring;
}

auto GraphicsRingItem::type() const -> int
{
    return Shape::RING;
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
    case GraphicsBasicItem::MouseRegion::DotRegion:
        switch (hoveredDotIndex()) {
        case 0: // 上点 - 垂直移动控制外圆半径 向上拖动增加半径，向下拖动减小半径
            ring.maxRadius -= dp.y();
            break;
        case 1: // 右点 - 水平移动控制外圆半径 向右拖动增加半径，向左拖动减小半径
            ring.maxRadius += dp.x();
            break;
        case 2: // 下点 - 垂直移动控制外圆半径 向下拖动增加半径，向上拖动减小半径
            ring.maxRadius += dp.y();
            break;
        case 3: // 左点 - 水平移动控制外圆半径 向左拖动增加半径，向右拖动减小半径
            ring.maxRadius -= dp.x(); //
            break;
        // 内圆对角线控制点 沿对角线方向
        case 4: // 右上点 - 对角线移动控制内圆半径
            ring.minRadius += (dp.x() - dp.y()) / std::sqrt(2);
            break;
        case 5: // 左上点 - 对角线移动控制内圆半径
            ring.minRadius += (-dp.x() - dp.y()) / std::sqrt(2);
            break;
        case 6: // 左下点 - 对角线移动控制内圆半径
            ring.minRadius += (-dp.x() + dp.y()) / std::sqrt(2);
            break;
        case 7: // 右下点 - 对角线移动控制内圆半径
            ring.minRadius += (dp.x() + dp.y()) / std::sqrt(2);
            break;
        default: break;
        }
        break;
    case GraphicsBasicItem::MouseRegion::All: ring.center += dp; break;
    case GraphicsBasicItem::MouseRegion::None:
        switch (d_ptr->mouseRegion) {
        case InEdge0:
            setMyCursor(ring.center, point);
            ring.minRadius = Utils::distance(ring.center, point);
            break;
        case InEdge1:
            setMyCursor(ring.center, point);
            ring.maxRadius = Utils::distance(ring.center, point);
            break;
        default: break;
        }
        break;
    default: break;
    }

    if (setRing(ring)) {
        update();
    }
}

void GraphicsRingItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF point = event->scenePos();
    auto pts_tmp = geometryCache()->controlPoints();
    int size = pts_tmp.size();
    if (size == 1 || size == 2) {
        pts_tmp.append(point);
        showHoverRing(pts_tmp);
    }
    if (!isValid()) {
        return;
    }
    GraphicsBasicItem::hoverMoveEvent(event);
    if (mouseRegion() == GraphicsBasicItem::MouseRegion::DotRegion) {
        return;
    }
    setMouseRegion(GraphicsBasicItem::MouseRegion::None);

    if (qAbs(Utils::distance(point, d_ptr->ring.center) - d_ptr->ring.minRadius) < margin() / 3) {
        d_ptr->mouseRegion = InEdge0;
        setMyCursor(d_ptr->ring.center, point);
    } else if (qAbs(Utils::distance(point, d_ptr->ring.center) - d_ptr->ring.maxRadius)
               < margin() / 3) {
        d_ptr->mouseRegion = InEdge1;
        setMyCursor(d_ptr->ring.center, point);
    } else if (shape().contains(point)) {
        setMouseRegion(GraphicsBasicItem::MouseRegion::All);
        setCursor(Qt::SizeAllCursor);
    } else {
        unsetCursor();
    }
}

void GraphicsRingItem::drawContent(QPainter *painter)
{
    if (isValid()) {
        painter->drawEllipse(d_ptr->ring.boundingRect(0));
        painter->drawEllipse(d_ptr->ring.minBoundingRect(0));
    } else {
        switch (geometryCache()->controlPoints().size()) {
        case 1: painter->drawEllipse(d_ptr->maxCircle.boundingRect(0)); break;
        case 2:
            painter->drawEllipse(d_ptr->tempRing.boundingRect(0));
            painter->drawEllipse(d_ptr->tempRing.minBoundingRect(0));
            break;
        default: break;
        }
    }
}

void GraphicsRingItem::pointsChanged(const QPolygonF &ply)
{
    auto rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }

    switch (ply.size()) {
    case 1: geometryCache()->setControlPoints(ply); break;
    case 2: { //外圈
        Circle circle(ply.first(), Utils::distance(ply.first(), ply.last()));
        if (rect.contains(circle.boundingRect(0)) && circle.isVaild(margin())) {
            d_ptr->maxCircle = circle;
            geometryCache()->setControlPoints(ply);
        } else {
            return;
        }
    } break;
    case 3:
        if (!setRing({d_ptr->maxCircle.center,
                      Utils::distance(d_ptr->maxCircle.center, ply[2]),
                      d_ptr->maxCircle.radius})) {
            return;
        }
        break;
    default: return;
    }
    update();
}

void GraphicsRingItem::showHoverRing(const QPolygonF &ply)
{
    switch (ply.size()) {
    case 2: {
        Circle circle(ply.first(), Utils::distance(ply.first(), ply.last()));
        d_ptr->maxCircle = circle;
    } break;
    case 3: {
        double minRadius = Utils::distance(d_ptr->maxCircle.center, ply[2]);
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

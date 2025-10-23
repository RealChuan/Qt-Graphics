#include "graphicslineitem.h"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

namespace Graphics {

namespace {

auto checkLineVaild(const QLineF &line, const double margin) -> bool
{
    return !line.isNull() && line.length() > margin;
}

auto lineShape(const QLineF &line, qreal extension) -> QPainterPath
{
    QPainterPath path;

    if (line.isNull())
        return path;

    // 计算线段的方向向量和长度
    QPointF dir = line.p2() - line.p1();
    qreal length = sqrt(dir.x() * dir.x() + dir.y() * dir.y());

    if (length == 0)
        return path;

    // 归一化方向向量
    dir /= length;

    // 计算法向量（垂直于线段方向）
    QPointF normal(-dir.y(), dir.x());

    // 延长后的端点
    QPointF extendedP1 = line.p1() - dir * extension;
    QPointF extendedP2 = line.p2() + dir * extension;

    // 计算侧向偏移量
    QPointF offset = normal * (extension);

    // 构造矩形路径
    path.moveTo(extendedP1 + offset);
    path.lineTo(extendedP2 + offset);
    path.lineTo(extendedP2 - offset);
    path.lineTo(extendedP1 - offset);
    path.closeSubpath();

    return path;
}

} // namespace

class GraphicsLineItem::GraphicsLineItemPrivate
{
public:
    explicit GraphicsLineItemPrivate(GraphicsLineItem *q)
        : q_ptr(q)
    {}

    GraphicsLineItem *q_ptr;

    QLineF line;
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

auto GraphicsLineItem::setLine(const QLineF &line) -> bool
{
    if (!checkLineVaild(line, margin())) {
        return false;
    }

    QPolygonF anchorPoints{line.p1(), line.p2()};
    auto rect = Utils::createBoundingRect(anchorPoints, margin());
    auto sceneRect = scene()->sceneRect();
    if (rect.isValid() && !sceneRect.contains(rect)) {
        return false;
    } else if (!sceneRect.contains(line.p1()) || !sceneRect.contains(line.p2())) {
        return false;
    }

    prepareGeometryChange();
    d_ptr->line = line;
    geometryCache()->setGeometryData(anchorPoints,
                                     Utils::createBoundingRect(anchorPoints, 0),
                                     lineShape(d_ptr->line, 6));
    emit lineChanged(line);

    return true;
}

auto GraphicsLineItem::line() const -> QLineF
{
    return d_ptr->line;
}

void GraphicsLineItem::drawContent(QPainter *painter)
{
    painter->drawLine(d_ptr->line);
}

void GraphicsLineItem::pointsChanged(const QPolygonF &ply)
{
    switch (ply.size()) {
    case 1: geometryCache()->setControlPoints(ply); break;
    case 2:
        if (!setLine(QLineF(ply[0], ply[1]))) {
            return;
        }
        break;
    default: return;
    }
    update();
}

void GraphicsLineItem::updateHoverPreview(const QPointF &scenePos)
{
    auto controlPoints = geometryCache()->controlPoints();
    if (controlPoints.size() != 1) {
        return;
    }
    auto line = QLineF(controlPoints[0], scenePos);
    if (!checkLineVaild(line, margin())) {
        return;
    }
    d_ptr->line = line;
    update();
}

void GraphicsLineItem::handleMouseMoveEvent(const QPointF &scenePos,
                                            const QPointF &clickedPos,
                                            const QPointF delta)
{
    auto controlPoints = geometryCache()->controlPoints();

    switch (mouseRegion()) {
    case MouseRegion::EntireShape: controlPoints.translate(delta); break;
    case MouseRegion::AnchorPoint: controlPoints.replace(hoveredDotIndex(), scenePos); break;
    default: return;
    }
    pointsChanged(controlPoints);
}

} // namespace Graphics

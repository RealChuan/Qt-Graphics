#include "graphicslineitem.h"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

namespace Graphics {

inline auto checkLineVaild(const QLineF &line, const double margin) -> bool
{
    return !line.isNull() && line.length() > margin;
}

inline auto lineShape(const QLineF &line, qreal penWidth) -> QPainterPath
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

    // 计算延长量
    qreal extension = penWidth;

    // 延长后的端点
    QPointF extendedP1 = line.p1() - dir * extension;
    QPointF extendedP2 = line.p2() + dir * extension;

    // 计算侧向偏移量
    QPointF offset = normal * (penWidth);

    // 构造矩形路径
    path.moveTo(extendedP1 + offset);
    path.lineTo(extendedP2 + offset);
    path.lineTo(extendedP2 - offset);
    path.lineTo(extendedP1 - offset);
    path.closeSubpath();

    return path;
}

class GraphicsLineItem::GraphicsLineItemPrivate
{
public:
    explicit GraphicsLineItemPrivate(GraphicsLineItem *q)
        : q_ptr(q)
    {}

    GraphicsLineItem *q_ptr;

    QLineF line;
    QLineF tempLine;
    QPainterPath shape;
    int lastAddLen = 0;
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
    if (!scene()->sceneRect().contains(rect)) {
        return false;
    }

    prepareGeometryChange();

    d_ptr->line = line;
    geometryCache()->setAnchorPoints(anchorPoints, Utils::createBoundingRect(anchorPoints, 0));
    emit lineChanged(line);

    d_ptr->lastAddLen = addLen(); // 线段的宽度
    d_ptr->shape = lineShape(line, d_ptr->lastAddLen);

    return true;
}

auto GraphicsLineItem::line() const -> QLineF
{
    return d_ptr->line;
}

auto GraphicsLineItem::shape() const -> QPainterPath
{
    if (isValid()) {
        auto add = addLen(); // 线段的宽度
        if (add != d_ptr->lastAddLen) {
            d_ptr->shape = lineShape(d_ptr->line, add);
            d_ptr->lastAddLen = add;
        }
        return d_ptr->shape;
    }
    return GraphicsBasicItem::shape();
}

auto GraphicsLineItem::type() const -> int
{
    return Shape::LINE;
}

void GraphicsLineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == 0 || !isValid()) {
        return;
    }
    if (!isSelected()) {
        setSelected(true);
    }
    auto scenePos = event->scenePos();
    auto dp = scenePos - clickedPos();
    setClickedPos(scenePos);

    auto pts_tmp = geometryCache()->anchorPoints();
    switch (mouseRegion()) {
    case MouseRegion::DotRegion: pts_tmp.replace(hoveredDotIndex(), scenePos); break;
    case MouseRegion::All: pts_tmp.translate(dp); break;
    default: return;
    }
    pointsChanged(pts_tmp);
}

void GraphicsLineItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    auto pts_tmp = geometryCache()->anchorPoints();
    if (pts_tmp.size() == 1) {
        pts_tmp.append(event->scenePos());
        showHoverLine(pts_tmp);
    }

    GraphicsBasicItem::hoverMoveEvent(event);
}

void GraphicsLineItem::drawContent(QPainter *painter)
{
    painter->drawLine(isValid() ? d_ptr->line : d_ptr->tempLine);
}

void GraphicsLineItem::pointsChanged(const QPolygonF &ply)
{
    auto rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }

    switch (ply.size()) {
    case 1: geometryCache()->setAnchorPoints(ply, {}); break;
    case 2:
        if (!setLine(QLineF(ply[0], ply[1]))) {
            return;
        }
        break;
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

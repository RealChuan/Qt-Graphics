#include "graphicsbasicitem.h"
#include "graphicsutils.hpp"

#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QWheelEvent>

namespace Graphics {

class GraphicsBasicItem::GraphicsBasicItemPrivate
{
public:
    explicit GraphicsBasicItemPrivate(GraphicsBasicItem *q)
        : q_ptr(q)
    {}

    GraphicsBasicItem *q_ptr;

    QString name;
    GraphicsBasicItem::MouseRegion mouseRegin = GraphicsBasicItem::None;
    int hoveredDotIndex = -1;
    QPointF clickedPos;
    QPolygonF cache;
    double margin = 10;
    bool showBoundingRect = false;
};

GraphicsBasicItem::GraphicsBasicItem(QGraphicsItem *parent)
    : QObject(nullptr)
    , QAbstractGraphicsShapeItem(parent)
    , d_ptr(new GraphicsBasicItemPrivate(this))
{
    setPen(QPen(Qt::blue));
    setFlags(flags() | ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges
             | ItemIsFocusable);
    setAcceptHoverEvents(true);
}

GraphicsBasicItem::~GraphicsBasicItem() {}

auto GraphicsBasicItem::boundingRect() const -> QRectF
{
    if (!isValid()) {
        return scene()->sceneRect();
    }

    QRectF rectF = d_ptr->cache.boundingRect();
    double addLen = d_ptr->margin / 2;
    rectF.adjust(-addLen, -addLen, addLen, addLen);
    return rectF;
}

void GraphicsBasicItem::setName(const QString &name)
{
    d_ptr->name = name;
}

auto GraphicsBasicItem::name() const -> QString
{
    return d_ptr->name;
}

void GraphicsBasicItem::setMargin(double m11)
{
    d_ptr->margin = 1.5 * 10 / m11;
    if (d_ptr->margin <= 3) {
        d_ptr->margin = 3;
    }

    if (d_ptr->margin >= 100) {
        d_ptr->margin = 100;
    }
}

auto GraphicsBasicItem::margin() const -> double
{
    return d_ptr->margin;
}

void GraphicsBasicItem::setItemEditable(bool editable)
{
    setAcceptHoverEvents(editable);
}

void GraphicsBasicItem::setShowBoundingRect(bool show)
{
    d_ptr->showBoundingRect = show;
    update();
}

bool GraphicsBasicItem::showBoundingRect() const
{
    return d_ptr->showBoundingRect;
}

//QVariant BasicGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change,
//                                       const QVariant &value)
//{
//    if (change == ItemPositionChange && scene()) {
//        // value is the new position.
//        QPointF newPos = value.toPointF();
//        QRectF rect = scene()->sceneRect();
//        if (!rect.contains(newPos)) {
//            // Keep the item inside the scene rect.
//            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
//            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
//            return newPos;
//        }
//    }
//    return QGraphicsItem::itemChange(change, value);
//}

void GraphicsBasicItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QAbstractGraphicsShapeItem::hoverMoveEvent(event);
    if (!isValid()) {
        return;
    }

    d_ptr->mouseRegin = None;
    d_ptr->hoveredDotIndex = -1;

    QPointF pos = event->scenePos();
    if (shape().contains(pos)) {
        d_ptr->mouseRegin = All;
        setCursor(Qt::SizeAllCursor);
    }

    for (const QPointF &p : std::as_const(d_ptr->cache)) {
        QPointF m(margin() / 2, margin() / 2);
        QRectF area(p - m, p + m);
        if (area.contains(pos)) {
            d_ptr->hoveredDotIndex = d_ptr->cache.indexOf(p);
            d_ptr->mouseRegin = DotRegion;
            setCursor(Qt::PointingHandCursor);
            return;
        }
    }
}

void GraphicsBasicItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    unsetCursor();
    d_ptr->mouseRegin = None;
    QAbstractGraphicsShapeItem::hoverLeaveEvent(event);
}

void GraphicsBasicItem::setCache(const QPolygonF &cache)
{
    d_ptr->cache = cache;
}

auto GraphicsBasicItem::cache() const -> QPolygonF
{
    return d_ptr->cache;
}

void GraphicsBasicItem::setClickedPos(const QPointF &pointF)
{
    d_ptr->clickedPos = pointF;
}

auto GraphicsBasicItem::clickedPos() const -> QPointF
{
    return d_ptr->clickedPos;
}

auto GraphicsBasicItem::mouseRegion() const -> GraphicsBasicItem::MouseRegion
{
    return d_ptr->mouseRegin;
}

void GraphicsBasicItem::setMouseRegion(const GraphicsBasicItem::MouseRegion mouseRegin)
{
    d_ptr->mouseRegin = mouseRegin;
}

auto GraphicsBasicItem::hoveredDotIndex() const -> int
{
    return d_ptr->hoveredDotIndex;
}

void GraphicsBasicItem::drawAnchor(QPainter *painter)
{
    if (!acceptHoverEvents()) {
        return;
    }

    QPolygonF polygonF = cache();
    for (const QPointF &p : std::as_const(polygonF)) {
        painter->fillRect(QRectF(p.x() - d_ptr->margin / 2,
                                 p.y() - d_ptr->margin / 2,
                                 d_ptr->margin,
                                 d_ptr->margin),
                          QColor(242, 80, 86));
    }
}

void GraphicsBasicItem::drawBoundingRect(QPainter *painter)
{
    if (!d_ptr->showBoundingRect || !isValid()) {
        return;
    }

    QPen outline(Qt::black, 1, Qt::DashLine);
    outline.setCosmetic(true);
    painter->setPen(outline);
    painter->drawRect(boundingRect());
}

void GraphicsBasicItem::setMyCursor(const QPointF &center, const QPointF &pos)
{
    double angle = QLineF(center, pos).angle();
    setCursor(Utils::curorFromAngle(Utils::ConvertTo360(angle - 90)));
}

} // namespace Graphics

#include "graphicsbasicitem.h"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QWheelEvent>

namespace Graphics {

class GraphicsBasicItem::GraphicsBasicItemPrivate
{
public:
    explicit GraphicsBasicItemPrivate(GraphicsBasicItem *q)
        : q_ptr(q)
        , geometryCachePtr(new GeometryCache)
    {}

    ~GraphicsBasicItemPrivate() {}

    GraphicsBasicItem *q_ptr;

    QString name;
    MouseRegion mouseRegin = MouseRegion::None;
    int hoveredDotIndex = -1;
    QPointF clickedPos;
    double margin = 6;
    bool showBoundingRect = true;
    const double minAddLen = 10;

    GeometryCachePtr geometryCachePtr;
};

GraphicsBasicItem::GraphicsBasicItem(QGraphicsItem *parent)
    : QObject(nullptr)
    , QAbstractGraphicsShapeItem(parent)
    , d_ptr(new GraphicsBasicItemPrivate(this))
{
    setPen(QPen(QColor(57, 163, 255), 2));
    setFlags(flags() | ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges
             | ItemIsFocusable);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setItemEditable(true);
}

GraphicsBasicItem::~GraphicsBasicItem() {}

auto GraphicsBasicItem::isValid() const -> bool
{
    return d_ptr->geometryCachePtr->isValid();
}

auto GraphicsBasicItem::boundingRect() const -> QRectF
{
    return d_ptr->geometryCachePtr->isValid()
               ? d_ptr->geometryCachePtr->boundingRect(margin() + addLen(), pen().width())
               : scene()->sceneRect();
}

void GraphicsBasicItem::setName(const QString &name)
{
    d_ptr->name = name;
}

auto GraphicsBasicItem::name() const -> QString
{
    return d_ptr->name;
}

void GraphicsBasicItem::setMargin(double margin)
{
    d_ptr->margin = margin;
}

auto GraphicsBasicItem::margin() const -> double
{
    return d_ptr->margin;
}

void GraphicsBasicItem::setItemEditable(bool editable)
{
    setAcceptHoverEvents(editable);
}

bool GraphicsBasicItem::itemEditable() const
{
    return acceptHoverEvents();
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

void GraphicsBasicItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return QAbstractGraphicsShapeItem::mousePressEvent(event);
    }
    setClickedPos(event->scenePos());
    if (isValid()) {
        return;
    }
    auto pts_tmp = d_ptr->geometryCachePtr->anchorPoints();
    pts_tmp.append(event->pos());
    pointsChanged(pts_tmp);
}

void GraphicsBasicItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QAbstractGraphicsShapeItem::hoverMoveEvent(event);
    if (!isValid()) {
        return;
    }

    d_ptr->mouseRegin = MouseRegion::None;
    d_ptr->hoveredDotIndex = -1;
    const auto scenePos = event->scenePos();
    const auto &anchorPoints = d_ptr->geometryCachePtr->anchorPoints();
    const qreal halfMargin = d_ptr->margin * 0.5;
    const QPointF marginOffset(halfMargin, halfMargin);

    for (int i = 0; i < anchorPoints.size(); ++i) {
        const auto &point = anchorPoints.at(i);
        const QRectF area(point - marginOffset, point + marginOffset);

        if (area.contains(scenePos)) {
            d_ptr->hoveredDotIndex = i;
            d_ptr->mouseRegin = MouseRegion::DotRegion;
            setCursor(Qt::PointingHandCursor);
            return;
        }
    }

    if (shape().contains(scenePos)) {
        d_ptr->mouseRegin = MouseRegion::All;
        setCursor(Qt::SizeAllCursor);
    }
}

void GraphicsBasicItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    unsetCursor();
    d_ptr->mouseRegin = MouseRegion::None;
    QAbstractGraphicsShapeItem::hoverLeaveEvent(event);
}

void GraphicsBasicItem::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(pen());

    drawContent(painter);

    if (option->state & QStyle::State_Selected) {
        drawAnchor(painter);
        drawBoundingRect(painter);
        drawShape(painter);
    }
}

GeometryCache *GraphicsBasicItem::geometryCache() const
{
    return d_ptr->geometryCachePtr.data();
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

void GraphicsBasicItem::drawContent(QPainter *painter) {}

void GraphicsBasicItem::drawAnchor(QPainter *painter)
{
    if (!acceptHoverEvents()) {
        return;
    }

    auto anchorPoints = d_ptr->geometryCachePtr->anchorPoints();
    for (const QPointF &p : std::as_const(anchorPoints)) {
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

void GraphicsBasicItem::drawShape(QPainter *painter)
{
    QPen outline(Qt::green, 1, Qt::DashLine);
    outline.setCosmetic(true);
    painter->setPen(outline);
    painter->drawPath(shape());
}

void GraphicsBasicItem::setMyCursor(const QPointF &center, const QPointF &pos)
{
    auto angle = QLineF(center, pos).angle();
    setCursor(Utils::cursorForDirection(angle - 90));
}

auto GraphicsBasicItem::addLen() const -> int
{
    return std::max({d_ptr->margin, pen().widthF(), d_ptr->minAddLen});
}

} // namespace Graphics

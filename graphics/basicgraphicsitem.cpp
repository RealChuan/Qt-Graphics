#include "basicgraphicsitem.h"
#include "graphics.h"

#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>
#include <QPainter>
#include <QWheelEvent>
#include <QMenu>

namespace Graphics {

class BasicGraphicsItem::BasicGraphicsItemPrivate
{
public:
    BasicGraphicsItemPrivate(QObject *parent)
        : owner(parent)
        , menu(new QMenu)
    {}

    QObject *owner;
    QString name;
    BasicGraphicsItem::MouseRegion mouseRegin = BasicGraphicsItem::None;
    int hoveredDotIndex = -1;
    QPointF clickedPos;
    QPolygonF cache;
    double margin = 10;
    QScopedPointer<QMenu> menu;
};

BasicGraphicsItem::BasicGraphicsItem(QGraphicsItem *parent)
    : QObject(nullptr)
    , QAbstractGraphicsShapeItem(parent)
    , d_ptr(new BasicGraphicsItemPrivate(this))
{
    setPen(QPen(Qt::blue));
    setFlags(ItemIsSelectable
             | ItemIsMovable
             | ItemSendsGeometryChanges
             | ItemIsFocusable );
    setAcceptHoverEvents(true);
    createPopMenu();
}

BasicGraphicsItem::~BasicGraphicsItem(){}

QRectF BasicGraphicsItem::boundingRect() const
{
    if (!isValid()) {
        return scene()->sceneRect();
    }

    QRectF rectF = d_ptr->cache.boundingRect();
    double addLen = d_ptr->margin / 2;
    rectF.adjust(-addLen, -addLen, addLen, addLen);
    return rectF;
}

void BasicGraphicsItem::setName(const QString &name)
{
    d_ptr->name = name;
}

QString BasicGraphicsItem::name() const
{
    return d_ptr->name;
}

void BasicGraphicsItem::setMargin(double m11)
{
    d_ptr->margin = 1.5 * 10 / m11;
    if (d_ptr->margin <= 3) {
        d_ptr->margin = 3;
    }

    if (d_ptr->margin >= 100) {
        d_ptr->margin = 100;
    }
}

double BasicGraphicsItem::margin() const
{
    return d_ptr->margin;
}

void BasicGraphicsItem::setItemEditable(bool editable)
{
    setAcceptHoverEvents(editable);
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

void BasicGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!isValid()) {
        return;
    }

    d_ptr->mouseRegin = None;
    d_ptr->hoveredDotIndex = -1;

    QPointF pos = event->scenePos();
    if(shape().contains(pos)){
        d_ptr->mouseRegin = All;
        setCursor(Qt::SizeAllCursor);
    }

    for (const QPointF &p : qAsConst(d_ptr->cache)) {
        QPointF m(margin() / 2, margin() / 2);
        QRectF area(p - m, p + m);
        if(area.contains(pos)){
            d_ptr->hoveredDotIndex = d_ptr->cache.indexOf(p);
            d_ptr->mouseRegin = DotRegion;
            setCursor(Qt::PointingHandCursor);
            return;
        }
    }
}

void BasicGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    unsetCursor();
    d_ptr->mouseRegin = None;
    QAbstractGraphicsShapeItem::hoverLeaveEvent(event);
}

void BasicGraphicsItem::keyPressEvent(QKeyEvent *event)
{
    if(!isSelected())
        return;
    switch (event->key()) {
    case Qt::Key_Delete: emit deleteMyself(); break;
    default: break;
    }
}

void BasicGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    d_ptr->menu->exec(event->screenPos());
}

void BasicGraphicsItem::setCache(const QPolygonF &cache)
{
    d_ptr->cache = cache;
}

QPolygonF BasicGraphicsItem::cache() const
{
    return d_ptr->cache;
}

void BasicGraphicsItem::setClickedPos(const QPointF &pointF)
{
    d_ptr->clickedPos = pointF;
}

QPointF BasicGraphicsItem::clickedPos() const
{
    return d_ptr->clickedPos;
}

BasicGraphicsItem::MouseRegion BasicGraphicsItem::mouseRegion() const
{
    return d_ptr->mouseRegin;
}

void BasicGraphicsItem::setMouseRegion(const BasicGraphicsItem::MouseRegion mouseRegin)
{
    d_ptr->mouseRegin = mouseRegin;
}

int BasicGraphicsItem::hoveredDotIndex() const
{
    return d_ptr->hoveredDotIndex;
}

void BasicGraphicsItem::drawAnchor(QPainter *painter)
{
    if (!acceptHoverEvents()) {
        return;
    }

    QPolygonF polygonF = cache();
    for (const QPointF &p : qAsConst(polygonF)) {
        painter->fillRect(QRectF(p.x() - d_ptr->margin / 2,
                                 p.y() - d_ptr->margin / 2,
                                 d_ptr->margin,
                                 d_ptr->margin),
                          QColor(242, 80, 86));
    }
}

void BasicGraphicsItem::setMyCursor(const QPointF &center, const QPointF &pos)
{
    double angle = QLineF(center, pos).angle();
    setCursor(Graphics::curorFromAngle(Graphics::ConvertTo360(angle - 90)));
}

void BasicGraphicsItem::createPopMenu()
{
    d_ptr->menu->addAction(tr("Delete"), this, &BasicGraphicsItem::deleteMyself);
}

}

#include "graphicstextitem.hpp"

namespace Graphics {

GraphicsTextItem::GraphicsTextItem(QGraphicsItem *parent)
    : QGraphicsTextItem(parent)
{
    setFlags(flags() | ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges
             | ItemIsFocusable);
}

auto GraphicsTextItem::itemChange(GraphicsItemChange change, const QVariant &value) -> QVariant
{
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        emit selectedChange(this);
    }
    return value;
}

void GraphicsTextItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}

void GraphicsTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction) {
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

} // namespace Graphics

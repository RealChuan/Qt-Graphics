#include "drawscene.hpp"

#include <graphics/graphicstextitem.hpp>

#include <QtWidgets>

void DrawScene::setTextColor(const QColor &color)
{
    m_textColor = color;

    auto selectedItems = this->selectedItems();
    for (auto item : selectedItems) {
        auto textItem = qgraphicsitem_cast<Graphics::GraphicsTextItem *>(item);
        if (textItem) {
            textItem->setDefaultTextColor(m_textColor);
        }
    }
}

void DrawScene::editorLostFocus(Graphics::GraphicsTextItem *item)
{
    auto cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}

void DrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        if (m_drawText) {
            auto textItem = new Graphics::GraphicsTextItem;
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setZValue(1000.0);
            textItem->setFont(font());
            textItem->setDefaultTextColor(m_textColor);
            connect(textItem,
                    &Graphics::GraphicsTextItem::selectedChange,
                    this,
                    &DrawScene::itemSelected);
            connect(textItem,
                    &Graphics::GraphicsTextItem::lostFocus,
                    this,
                    &DrawScene::editorLostFocus);
            addItem(textItem);
            textItem->setPos(mouseEvent->scenePos());
            m_drawText = false;
        }
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void DrawScene::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Delete: emit deleteItem(); break;
    default: break;
    }

    QGraphicsScene::keyPressEvent(event);
}

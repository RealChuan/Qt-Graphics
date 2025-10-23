#pragma once

#include "graphics_global.h"

#include <QGraphicsTextItem>

namespace Graphics {

class GRAPHICS_EXPORT GraphicsTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 1 };

    explicit GraphicsTextItem(QGraphicsItem *parent = nullptr);

    [[nodiscard]] auto type() const -> int override { return Type; }

signals:
    void lostFocus(Graphics::GraphicsTextItem *item);
    void selectedChange(Graphics::GraphicsTextItem *item);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    auto itemChange(GraphicsItemChange change, const QVariant &value) -> QVariant override;

    void focusOutEvent(QFocusEvent *event) override;
};

} // namespace Graphics

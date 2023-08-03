#ifndef GRAPHICSTEXTITEM_HPP
#define GRAPHICSTEXTITEM_HPP

#include <QGraphicsTextItem>

#include "graphics_global.h"

namespace Graphics {

class GRAPHICS_EXPORT GraphicsTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 3 };

    explicit GraphicsTextItem(QGraphicsItem *parent = nullptr);

    [[nodiscard]] auto type() const -> int override { return Type; }

signals:
    void lostFocus(Graphics::GraphicsTextItem *item);
    void selectedChange(QGraphicsItem *item);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    auto itemChange(GraphicsItemChange change, const QVariant &value) -> QVariant override;

    void focusOutEvent(QFocusEvent *event) override;
};

} // namespace Graphics

#endif // GRAPHICSTEXTITEM_HPP

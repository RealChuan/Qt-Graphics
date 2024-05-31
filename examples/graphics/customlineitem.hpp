#ifndef CUSTOMLINEITEM_HPP
#define CUSTOMLINEITEM_HPP

#include <graphics/graphicslineitem.h>

class CustomLineItem : public Graphics::GraphicsLineItem
{
public:
    using Graphics::GraphicsLineItem::GraphicsLineItem;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // CUSTOMLINEITEM_HPP

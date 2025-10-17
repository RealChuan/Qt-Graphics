#pragma once

#include <graphics/graphicslineitem.h>

class CustomLineItem : public Graphics::GraphicsLineItem
{
public:
    using Graphics::GraphicsLineItem::GraphicsLineItem;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
};

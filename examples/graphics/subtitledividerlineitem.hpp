#pragma once

#include <graphics/graphicslineitem.h>

class SubtitleDividerLineItem : public Graphics::GraphicsLineItem
{
public:
    explicit SubtitleDividerLineItem(QGraphicsItem *parent = nullptr);

protected:
    void handleMouseMoveEvent(const QPointF &scenePos,
                              const QPointF &clickedPos,
                              const QPointF delta) override;
};

using SubtitleDividerLineItemPtr = QScopedPointer<SubtitleDividerLineItem>;

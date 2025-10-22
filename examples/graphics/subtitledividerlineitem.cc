#include "subtitledividerlineitem.hpp"

#include <graphics/geometrycache.hpp>

SubtitleDividerLineItem::SubtitleDividerLineItem(QGraphicsItem *parent)
    : GraphicsLineItem(parent)
{
    setMargin(0.5);
    setShowBoundingRect(false);
    setShowShape(false);
}

void SubtitleDividerLineItem::handleMouseMoveEvent(const QPointF &scenePos,
                                                   const QPointF &clickedPos,
                                                   const QPointF delta)
{
    auto dp = QPointF(0, delta.y());
    auto controlPoints = geometryCache()->controlPoints();

    switch (mouseRegion()) {
    case MouseRegion::EntireShape: controlPoints.translate(dp); break;
    default: return;
    }
    pointsChanged(controlPoints);
}

#include "customlineitem.hpp"

#include <graphics/geometrycache.hpp>

#include <QGraphicsSceneMouseEvent>

void CustomLineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == 0 || !isValid()) {
        return;
    }
    if (!isSelected()) {
        setSelected(true);
    }
    QPointF point = event->scenePos();
    auto pts_tmp = geometryCache()->controlPoints();
    QPointF dp = point - clickedPos();
    dp = QPointF(0, dp.y());
    setClickedPos(point);

    switch (mouseRegion()) {
    case All: pts_tmp.translate(dp); break;
    default: return;
    }
    pointsChanged(pts_tmp);
}

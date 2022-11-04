#ifndef GRAPHICSROTATEDRECTITEM_H
#define GRAPHICSROTATEDRECTITEM_H

#include "basicgraphicsitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT RotatedRect
{
    bool isValid() const;

    QPointF center;
    double width = 0;
    double height = 0;
    double angle = 0;
};

class GRAPHICS_EXPORT GraphicsRotatedRectItem : public BasicGraphicsItem
{
public:
    explicit GraphicsRotatedRectItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsRotatedRectItem(const RotatedRect &rotatedRect,
                                     QGraphicsItem *parent = nullptr);
    ~GraphicsRotatedRectItem() override;

    void setRotatedRect(const RotatedRect &rotatedRect);
    RotatedRect rotatedRect() const;

    bool isValid() const override;
    int type() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
private:
    void pointsChanged(const QPolygonF &ply);

    struct GraphicsRotatedRectItemPrivate;
    QScopedPointer<GraphicsRotatedRectItemPrivate> d_ptr;
};

}

#endif // GRAPHICSROTATEDRECTITEM_H

#ifndef GRAPHICSCIRCLEITEM_H
#define GRAPHICSCIRCLEITEM_H

#include "basicgraphicsitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT Circle{
    bool isVaild() const;
    QRectF boundingRect() const;

    QPointF center;
    double radius = 0;
};

class GRAPHICS_EXPORT GraphicsCircleItem : public BasicGraphicsItem
{
public:
    explicit GraphicsCircleItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsCircleItem(const Circle &, QGraphicsItem *parent = nullptr);
    ~GraphicsCircleItem() override;

    static bool checkCircle(const Circle &circle, const double margin);

    void setCircle(const Circle&);
    Circle circle() const;

    bool isValid() const override;
    int type() const override;
    QPainterPath shape() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
private:
    void pointsChanged(const QPolygonF &ply);
    void showHoverCircle(const QPolygonF &ply);

    struct GraphicsCircleItemPrivate;
    QScopedPointer<GraphicsCircleItemPrivate> d_ptr;
};

}

#endif // GRAPHICSCIRCLEITEM_H

#ifndef GRAPHICSLINEITEM_H
#define GRAPHICSLINEITEM_H

#include "basicgraphicsitem.h"

namespace Graphics {

class GRAPHICS_EXPORT GraphicsLineItem : public BasicGraphicsItem
{
public:
    explicit GraphicsLineItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsLineItem(const QLineF &line, QGraphicsItem *parent = nullptr);
    ~GraphicsLineItem() override;

    void setLine(const QLineF &line);
    QLineF line() const;

    bool isValid() const override;
    int type() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    void pointsChanged(const QPolygonF &ply);

private:
    void showHoverLine(const QPolygonF &ply);

    struct GraphicsLineItemPrivate;
    QScopedPointer<GraphicsLineItemPrivate> d_ptr;
};

} // namespace Graphics

#endif // GRAPHICSLINEITEM_H

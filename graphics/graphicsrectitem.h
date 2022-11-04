#ifndef GRAPHICSRECTITEM_H
#define GRAPHICSRECTITEM_H

#include "basicgraphicsitem.h"

namespace Graphics {

class GRAPHICS_EXPORT GraphicsRectItem : public BasicGraphicsItem
{
    Q_OBJECT
public:
    explicit GraphicsRectItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsRectItem(const QRectF &, QGraphicsItem *parent = nullptr);
    ~GraphicsRectItem() override;

    void setRect(const QRectF &);
    QRectF rect() const;

    bool isValid() const override;
    int type() const override;

signals:
    void rectChanged(const QRectF &rectF);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    void pointsChanged(const QPolygonF &ply);
    void showHoverRect(const QPolygonF &ply);

    struct GraphicsRectItemPrivate;
    QScopedPointer<GraphicsRectItemPrivate> d_ptr;
};

} // namespace Graphics

#endif // GRAPHICSRECTITEM_H

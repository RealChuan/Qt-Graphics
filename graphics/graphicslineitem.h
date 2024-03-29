#ifndef GRAPHICSLINEITEM_H
#define GRAPHICSLINEITEM_H

#include "basicgraphicsitem.h"

namespace Graphics {

class GRAPHICS_EXPORT GraphicsLineItem : public BasicGraphicsItem
{
    Q_OBJECT
public:
    explicit GraphicsLineItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsLineItem(const QLineF &line, QGraphicsItem *parent = nullptr);
    ~GraphicsLineItem() override;

    void setLine(const QLineF &line);
    [[nodiscard]] auto line() const -> QLineF;

    [[nodiscard]] auto isValid() const -> bool override;
    [[nodiscard]] auto type() const -> int override;

signals:
    void lineChnaged(const QLineF &line);

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

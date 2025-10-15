#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {
class GRAPHICS_EXPORT GraphicsPolygonItem : public GraphicsBasicItem
{
public:
    explicit GraphicsPolygonItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsPolygonItem(const QPolygonF &polygon, QGraphicsItem *parent = nullptr);
    ~GraphicsPolygonItem() override;

    void setPolygon(const QPolygonF &ply);
    [[nodiscard]] auto polygon() const -> QPolygonF;

    [[nodiscard]] auto isValid() const -> bool override;
    [[nodiscard]] auto type() const -> int override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    void pointsChanged(const QPolygonF &ply);
    void showHoverPolygon(const QPolygonF &ply);

    class GraphicsPolygonItemPrivate;
    QScopedPointer<GraphicsPolygonItemPrivate> d_ptr;
};

} // namespace Graphics

#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {
class GRAPHICS_EXPORT GraphicsPolygonItem : public GraphicsBasicItem
{
public:
    explicit GraphicsPolygonItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsPolygonItem(const QPolygonF &polygon, QGraphicsItem *parent = nullptr);
    ~GraphicsPolygonItem() override;

    [[nodiscard]] auto setPolygon(const QPolygonF &ply) -> bool;
    [[nodiscard]] auto polygon() const -> QPolygonF;

    [[nodiscard]] auto type() const -> int override;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;

private:
    void showHoverPolygon(const QPolygonF &ply);

    class GraphicsPolygonItemPrivate;
    QScopedPointer<GraphicsPolygonItemPrivate> d_ptr;
};

} // namespace Graphics

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

    [[nodiscard]] auto type() const -> int override { return Shape::POLYGON; }

protected:
    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;
    void updateHoverPreview(const QPointF &scenePos) override;
    void handleMouseMoveEvent(const QPointF &scenePos,
                              const QPointF &clickedPos,
                              const QPointF delta) override;

private:
    class GraphicsPolygonItemPrivate;
    QScopedPointer<GraphicsPolygonItemPrivate> d_ptr;
};

} // namespace Graphics

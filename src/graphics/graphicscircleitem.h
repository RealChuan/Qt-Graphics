#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT Circle
{
    [[nodiscard]] auto isVaild(double margin) const -> bool;
    [[nodiscard]] auto boundingRect(double margin) const -> QRectF;
    [[nodiscard]] auto controlPoints() const -> QPolygonF;

    QPointF center;
    double radius = 0;
};

class GRAPHICS_EXPORT GraphicsCircleItem : public GraphicsBasicItem
{
public:
    explicit GraphicsCircleItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsCircleItem(const Circle &, QGraphicsItem *parent = nullptr);
    ~GraphicsCircleItem() override;

    [[nodiscard]] auto setCircle(const Circle &circle) -> bool;
    [[nodiscard]] auto circle() const -> Circle;

    [[nodiscard]] auto type() const -> int override { return Shape::CIRCLE; }

protected:
    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;
    void updateHoverPreview(const QPointF &scenePos) override;
    MouseRegion detectEdgeRegion(const QPointF &scenePos) override;
    void handleMouseMoveEvent(const QPointF &scenePos,
                              const QPointF &clickedPos,
                              const QPointF delta) override;

private:
    class GraphicsCircleItemPrivate;
    QScopedPointer<GraphicsCircleItemPrivate> d_ptr;
};

} // namespace Graphics

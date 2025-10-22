#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT Ring
{
    [[nodiscard]] auto boundingRect(double margin) const -> QRectF;
    [[nodiscard]] auto minBoundingRect(double margin) const -> QRectF;
    [[nodiscard]] auto isVaild(double margin) const -> bool;
    [[nodiscard]] auto controlPoints() const -> QPolygonF;

    QPointF center;
    double minRadius = 0;
    double maxRadius = 0;
};

class GRAPHICS_EXPORT GraphicsRingItem : public GraphicsBasicItem
{
public:
    explicit GraphicsRingItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsRingItem(const Ring &ring, QGraphicsItem *parent = nullptr);
    ~GraphicsRingItem() override;

    [[nodiscard]] auto setRing(const Ring &ring) -> bool;
    [[nodiscard]] auto ring() const -> Ring;

    [[nodiscard]] auto type() const -> int override { return Shape::RING; }

protected:
    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;
    void updateHoverPreview(const QPointF &scenePos) override;
    GraphicsBasicItem::MouseRegion detectEdgeRegion(const QPointF &scenePos) override;
    void handleMouseMoveEvent(const QPointF &scenePos,
                              const QPointF &clickedPos,
                              const QPointF delta) override;

private:
    class GraphicsRingItemPrivate;
    QScopedPointer<GraphicsRingItemPrivate> d_ptr;
};

} // namespace Graphics

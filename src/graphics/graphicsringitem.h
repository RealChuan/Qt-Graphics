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
    enum MouseRegion { InEdge0, InEdge1, None };
    explicit GraphicsRingItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsRingItem(const Ring &ring, QGraphicsItem *parent = nullptr);
    ~GraphicsRingItem() override;

    [[nodiscard]] auto setRing(const Ring &ring) -> bool;
    [[nodiscard]] auto ring() const -> Ring;

    [[nodiscard]] auto type() const -> int override;
    [[nodiscard]] auto shape() const -> QPainterPath override;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;

private:
    void showHoverRing(const QPolygonF &ply);

    class GraphicsRingItemPrivate;
    QScopedPointer<GraphicsRingItemPrivate> d_ptr;
};

} // namespace Graphics

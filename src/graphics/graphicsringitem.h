#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT Ring
{
    [[nodiscard]] auto boundingRect() const -> QRectF;
    [[nodiscard]] auto minBoundingRect() const -> QRectF;
    [[nodiscard]] auto isVaild() const -> bool;

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

    void setRing(const Ring &);
    [[nodiscard]] auto ring() const -> Ring;

    [[nodiscard]] auto isValid() const -> bool override;
    [[nodiscard]] auto type() const -> int override;
    [[nodiscard]] auto shape() const -> QPainterPath override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    void pointsChanged(const QPolygonF &ply);
    void showHoverRing(const QPolygonF &ply);

    class GraphicsRingItemPrivate;
    QScopedPointer<GraphicsRingItemPrivate> d_ptr;
};

} // namespace Graphics

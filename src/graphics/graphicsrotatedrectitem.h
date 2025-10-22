#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT RotatedRect
{
    [[nodiscard]] auto isValid(double margin) const -> bool;
    [[nodiscard]] auto boundingRect(double margin) const -> QRectF;
    [[nodiscard]] auto controlPoints() const -> QPolygonF;
    [[nodiscard]] auto rotationLine() const -> QLineF;

    QPointF center;
    double width = 0;
    double height = 0;
    double angle = 0;
};

class GRAPHICS_EXPORT GraphicsRotatedRectItem : public GraphicsBasicItem
{
public:
    explicit GraphicsRotatedRectItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsRotatedRectItem(const RotatedRect &rotatedRect,
                                     QGraphicsItem *parent = nullptr);
    ~GraphicsRotatedRectItem() override;

    [[nodiscard]] auto setRotatedRect(const RotatedRect &rotatedRect) -> bool;
    [[nodiscard]] auto rotatedRect() const -> RotatedRect;

    [[nodiscard]] auto type() const -> int override { return Shape::ROTATEDRECT; }

protected:
    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;
    void updateHoverPreview(const QPointF &scenePos) override;
    MouseRegion detectEdgeRegion(const QPointF &scenePos) override;
    void handleMouseMoveEvent(const QPointF &scenePos,
                              const QPointF &clickedPos,
                              const QPointF delta) override;

private:
    class GraphicsRotatedRectItemPrivate;
    QScopedPointer<GraphicsRotatedRectItemPrivate> d_ptr;
};

} // namespace Graphics

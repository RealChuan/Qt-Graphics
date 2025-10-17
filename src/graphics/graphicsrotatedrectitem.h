#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT RotatedRect
{
    [[nodiscard]] auto isValid(double margin) const -> bool;
    [[nodiscard]] auto controlPoints() const -> QPolygonF;

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

    [[nodiscard]] auto type() const -> int override;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;

private:
    class GraphicsRotatedRectItemPrivate;
    QScopedPointer<GraphicsRotatedRectItemPrivate> d_ptr;
};

} // namespace Graphics

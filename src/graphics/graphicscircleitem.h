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

    [[nodiscard]] auto type() const -> int override;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;

private:
    void showHoverCircle(const QPolygonF &ply);

    class GraphicsCircleItemPrivate;
    QScopedPointer<GraphicsCircleItemPrivate> d_ptr;
};

} // namespace Graphics

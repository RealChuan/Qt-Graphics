#pragma once
#include "graphicsbasicitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT Circle
{
    [[nodiscard]] auto isVaild() const -> bool;
    [[nodiscard]] auto boundingRect() const -> QRectF;

    QPointF center;
    double radius = 0;
};

class GRAPHICS_EXPORT GraphicsCircleItem : public GraphicsBasicItem
{
public:
    explicit GraphicsCircleItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsCircleItem(const Circle &, QGraphicsItem *parent = nullptr);
    ~GraphicsCircleItem() override;

    static auto checkCircle(const Circle &circle, const double margin) -> bool;

    void setCircle(const Circle &);
    [[nodiscard]] auto circle() const -> Circle;

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
    void showHoverCircle(const QPolygonF &ply);

    class GraphicsCircleItemPrivate;
    QScopedPointer<GraphicsCircleItemPrivate> d_ptr;
};

} // namespace Graphics

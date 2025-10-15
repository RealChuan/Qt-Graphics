#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT Arc
{
    [[nodiscard]] auto isValid() const -> bool;

    QPointF center;
    double minRadius = 0;
    double maxRadius = 0;
    double startAngle = 0;
    double endAngle = 0;
};

class GRAPHICS_EXPORT GraphicsArcItem : public GraphicsBasicItem
{
public:
    enum MouseRegion { InEdge0, InEdge1, None, InEdgeL, InEdgeH };

    explicit GraphicsArcItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsArcItem(const Arc &arc, QGraphicsItem *parent = nullptr);
    ~GraphicsArcItem() override;

    void setArc(const Arc &arc);
    [[nodiscard]] auto arch() const -> Arc;

    [[nodiscard]] auto isValid() const -> bool override;
    [[nodiscard]] auto type() const -> int override;
    [[nodiscard]] auto boundingRect() const -> QRectF override;
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
    void showHoverArc(const QPolygonF &ply);

private:
    class GraphicsArcItemPrivate;
    QScopedPointer<GraphicsArcItemPrivate> d_ptr;
};

} // namespace Graphics

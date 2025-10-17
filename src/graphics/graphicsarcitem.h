#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT Arc
{
    [[nodiscard]] auto isValid(double margin) const -> bool;
    [[nodiscard]] auto controlPoints() const -> QPolygonF;

    QPointF center;
    double minRadius = 0;
    double maxRadius = 0;
    double startAngle = 0;
    double endAngle = 0;
};

class GRAPHICS_EXPORT GraphicsArcItem : public GraphicsBasicItem
{
public:
    enum MouseRegion : int { InEdge0, InEdge1, None, InEdgeL, InEdgeH };

    explicit GraphicsArcItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsArcItem(const Arc &arc, QGraphicsItem *parent = nullptr);
    ~GraphicsArcItem() override;

    [[nodiscard]] auto setArc(const Arc &arc) -> bool;
    [[nodiscard]] auto arch() const -> Arc;

    [[nodiscard]] auto type() const -> int override;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;

private:
    void showHoverArc(const QPolygonF &ply);

    class GraphicsArcItemPrivate;
    QScopedPointer<GraphicsArcItemPrivate> d_ptr;
};

} // namespace Graphics

#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {

class GRAPHICS_EXPORT GraphicsLineItem : public GraphicsBasicItem
{
    Q_OBJECT
public:
    explicit GraphicsLineItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsLineItem(const QLineF &line, QGraphicsItem *parent = nullptr);
    ~GraphicsLineItem() override;

    [[nodiscard]] auto setLine(const QLineF &line) -> bool;
    [[nodiscard]] auto line() const -> QLineF;

    [[nodiscard]] auto type() const -> int override { return Shape::LINE; }

signals:
    void lineChanged(const QLineF &line);

protected:
    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;
    void updateHoverPreview(const QPointF &scenePos) override;
    void handleMouseMoveEvent(const QPointF &scenePos,
                              const QPointF &clickedPos,
                              const QPointF delta) override;

private:
    class GraphicsLineItemPrivate;
    QScopedPointer<GraphicsLineItemPrivate> d_ptr;
};

} // namespace Graphics

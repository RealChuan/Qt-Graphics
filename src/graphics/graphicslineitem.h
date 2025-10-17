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

    [[nodiscard]] auto shape() const -> QPainterPath override;
    [[nodiscard]] auto type() const -> int override;

signals:
    void lineChanged(const QLineF &line);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;

private:
    void showHoverLine(const QPolygonF &ply);

    class GraphicsLineItemPrivate;
    QScopedPointer<GraphicsLineItemPrivate> d_ptr;
};

} // namespace Graphics

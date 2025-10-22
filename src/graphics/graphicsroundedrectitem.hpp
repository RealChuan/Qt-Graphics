#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT RoundedRect
{
    RoundedRect() = default;
    RoundedRect(const QRectF &rect, qreal xRadius = 0, qreal yRadius = 0);

    bool isValid(double margin) const;

    QRectF rect;
    qreal xRadius = 6;
    qreal yRadius = 6;
};

class GRAPHICS_EXPORT GraphicsRoundedRectItem : public GraphicsBasicItem
{
    Q_OBJECT
public:
    explicit GraphicsRoundedRectItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsRoundedRectItem(const RoundedRect &roundedRect,
                                     QGraphicsItem *parent = nullptr);
    virtual ~GraphicsRoundedRectItem() override;

    [[nodiscard]] auto setRoundedRect(const RoundedRect &roundedRect) -> bool;
    [[nodiscard]] auto roundedRect() const -> RoundedRect { return m_roundedRect; }

    [[nodiscard]] auto type() const -> int override { return Shape::ROUNDEDRECT; }

signals:
    void roundedRectChanged(const RoundedRect &roundedRect);

protected:
    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;
    void updateHoverPreview(const QPointF &scenePos) override;
    MouseRegion detectEdgeRegion(const QPointF &scenePos) override;
    void handleMouseMoveEvent(const QPointF &scenePos,
                              const QPointF &clickedPos,
                              const QPointF delta) override;

    RoundedRect m_roundedRect;

private:
    class GraphicsRoundedRectItemPrivate;
    QScopedPointer<GraphicsRoundedRectItemPrivate> d_ptr;
};

} // namespace Graphics

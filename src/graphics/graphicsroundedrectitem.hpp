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
    [[nodiscard]] auto roundedRect() const -> RoundedRect;

    [[nodiscard]] auto type() const -> int override;

signals:
    void roundedRectChanged(const RoundedRect &roundedRect);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    void drawContent(QPainter *painter) override;
    void pointsChanged(const QPolygonF &ply) override;

    RoundedRect m_roundedRect;
    RoundedRect m_tempRoundedRect;

private:
    void showHoverRect(const QPolygonF &ply);

    class GraphicsRoundedRectItemPrivate;
    QScopedPointer<GraphicsRoundedRectItemPrivate> d_ptr;
};

} // namespace Graphics

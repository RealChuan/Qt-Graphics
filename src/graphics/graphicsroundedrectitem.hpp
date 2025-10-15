#pragma once

#include "graphicsbasicitem.h"

namespace Graphics {

struct GRAPHICS_EXPORT RoundedRect
{
    RoundedRect() = default;
    RoundedRect(const QRectF &rect, qreal xRadius = 0, qreal yRadius = 0);

    bool isValid() const;

    QRectF rect;
    qreal xRadius = 10;
    qreal yRadius = 10;
};

class GRAPHICS_EXPORT GraphicsRoundedRectItem : public GraphicsBasicItem
{
    Q_OBJECT
public:
    explicit GraphicsRoundedRectItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsRoundedRectItem(const RoundedRect &roundedRect,
                                     QGraphicsItem *parent = nullptr);
    virtual ~GraphicsRoundedRectItem() override;

    void setRoundedRect(const RoundedRect &roundedRect);
    [[nodiscard]] auto roundedRect() const -> RoundedRect;

    [[nodiscard]] auto isValid() const -> bool override;
    [[nodiscard]] auto type() const -> int override;

signals:
    void roundedRectChanged(const RoundedRect &roundedRect);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    RoundedRect m_roundedRect;
    RoundedRect m_tempRoundedRect;

private:
    void pointsChanged(const QPolygonF &ply);
    void showHoverRect(const QPolygonF &ply);

    class GraphicsRoundedRectItemPrivate;
    QScopedPointer<GraphicsRoundedRectItemPrivate> d_ptr;
};

} // namespace Graphics

#pragma once

#include "graphicsroundedrectitem.hpp"

namespace Graphics {

class GRAPHICS_EXPORT GraphicsRectItem : public GraphicsRoundedRectItem
{
    Q_OBJECT
public:
    explicit GraphicsRectItem(QGraphicsItem *parent = nullptr);
    explicit GraphicsRectItem(const QRectF &rect, QGraphicsItem *parent = nullptr);
    ~GraphicsRectItem() override;

    void setRect(const QRectF &rect);
    [[nodiscard]] auto rect() const -> QRectF;

    [[nodiscard]] auto isValid() const -> bool override;
    [[nodiscard]] auto type() const -> int override;

signals:
    void rectChanged(const QRectF &rectF);

private slots:
    void onRoundedRectChanged(const RoundedRect &roundedRect);

private:
    void buildConnect();
};

} // namespace Graphics

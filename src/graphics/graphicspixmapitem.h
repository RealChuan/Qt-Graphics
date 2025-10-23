#pragma once

#include "graphics_global.h"

#include <QGraphicsPixmapItem>

namespace Graphics {

class GRAPHICS_EXPORT GraphicsPixmapItem : public QGraphicsPixmapItem
{
public:
    enum class MaskEditingMode : int { Normal, Erase, Draw };

    explicit GraphicsPixmapItem(QGraphicsItem *parent = nullptr);
    ~GraphicsPixmapItem() override;

    void setSourcePixmap(const QPixmap &pixmap);

    void setMaskImage(const QImage &mask);
    [[nodiscard]] auto maskImage() const -> QImage;

    void setMaskEditingMode(MaskEditingMode mode);
    [[nodiscard]] auto maskEditingMode() const -> MaskEditingMode;

    void setBrushSize(int size);
    [[nodiscard]] auto brushSize() const -> int;

    void setMaskOpacity(double opacity);
    auto maskOpacity() const -> double;

    void setCheckerColor1(const QColor &color);
    auto checkerColor1() const -> QColor;

    void setCheckerColor2(const QColor &color);
    auto checkerColor2() const -> QColor;

    void resetMask();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void updateMaskWithBrushStroke();
    void updateCursor();

    class GraphicsPixmapItemPrivate;
    QScopedPointer<GraphicsPixmapItemPrivate> d_ptr;
};

} // namespace Graphics

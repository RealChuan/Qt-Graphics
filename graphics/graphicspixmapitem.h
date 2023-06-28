#ifndef GRAPHICSPIXMAPITEM_H
#define GRAPHICSPIXMAPITEM_H

#include "graphics_global.h"

#include <QGraphicsPixmapItem>

namespace Graphics {

class GRAPHICS_EXPORT GraphicsPixmapItem : public QGraphicsPixmapItem
{
public:
    enum Mode { Normal, MaskErase, MaskDraw };

    explicit GraphicsPixmapItem(QGraphicsItem *parent = nullptr);
    ~GraphicsPixmapItem() override;

    void setCustomPixmap(const QPixmap &pixmap);

    void setMaskImage(const QImage &mask);
    [[nodiscard]] auto maskImage() const -> QImage;

    void setPaintMode(Mode mode);
    [[nodiscard]] auto paintMode() const -> Mode;

    void setPenSize(int size);
    [[nodiscard]] auto penSize() const -> int;

    void setOpacity(double opacity);
    auto opacity() -> double;

    void setMaskColor1(const QColor &color);
    auto maskColor1() -> QColor;

    void setMaskColor12(const QColor &color);
    auto maskColor2() -> QColor;

    void clearMask();

protected:
    void setCursorPixmap();
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void paintImage();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    class GraphicsPixmapItemPrivate;
    QScopedPointer<GraphicsPixmapItemPrivate> d_ptr;
};

}

#endif // GRAPHICSPIXMAPITEM_H

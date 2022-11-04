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
    ~GraphicsPixmapItem();

    void setCustomPixmap(const QPixmap &pixmap);

    void setMaskImage(const QImage &mask);
    QImage maskImage() const;

    void setPaintMode(Mode mode);
    Mode paintMode() const;

    void setPenSize(int size);
    int penSize() const;

    void setOpacity(double opacity);
    double opacity();

    void setMaskColor1(const QColor &color);
    QColor maskColor1();

    void setMaskColor12(const QColor &color);
    QColor maskColor2();

    void clearMask();

protected:
    void setCursorPixmap();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void paintImage();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    class GraphicsPixmapItemPrivate;
    QScopedPointer<GraphicsPixmapItemPrivate> d_ptr;
};

}

#endif // GRAPHICSPIXMAPITEM_H

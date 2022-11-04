#include "graphicspixmapitem.h"

#include <QCursor>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace Graphics {

class GraphicsPixmapItem::GraphicsPixmapItemPrivate
{
public:
    GraphicsPixmapItemPrivate(QGraphicsItem *parent)
        : owner(parent)
    {}
    QGraphicsItem *owner;

    GraphicsPixmapItem::Mode mode = GraphicsPixmapItem::Mode::Normal;
    int penSize = 50;
    double scaleFactor = 1.0;
    QImage mask;
    QColor color = QColor(200, 0, 200);
    QPointF lastPos;
    QPointF currPos;
    double opacity = 0.5;

    QColor color1 = QColor(220, 220, 220);
    QColor color2 = Qt::white;
    QPixmap maskPixmap; // 也可用自定义的图片
};

GraphicsPixmapItem::GraphicsPixmapItem(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , d_ptr(new GraphicsPixmapItemPrivate(this))
{
    setAcceptHoverEvents(true);
    setFlags(flags() | ItemIsSelectable);
    setCacheMode(QGraphicsItem::NoCache);
    setTransformationMode(Qt::SmoothTransformation);
    setZValue(0);
}

GraphicsPixmapItem::~GraphicsPixmapItem() {}

void GraphicsPixmapItem::setCustomPixmap(const QPixmap &pixmap)
{
    setPixmap(pixmap);
    if (pixmap.size() != d_ptr->mask.size())
        clearMask();
    setPenSize(qMin(pixmap.width(), pixmap.height()) / 20.0);
}

void GraphicsPixmapItem::setMaskImage(const QImage &mask)
{
    if (mask.isNull() || pixmap().isNull()) {
        return;
    }
    if (pixmap().size() != mask.size()) {
        return;
    }
    // generate 4 channel color mask image
    QImage temp = QImage(pixmap().size(), QImage::Format_ARGB32);
    temp.fill(Qt::transparent);
    QRgb dstRgb = d_ptr->color.rgb();
    int cols = temp.width();
    int rows = temp.height();
    for (int y = 0; y < rows; y++) {
        QRgb *dst_row = (QRgb *) temp.scanLine(y);
        const uchar *mask_row = mask.scanLine(y);
        for (int x = 0; x < cols; x++) {
            if (mask_row[x] != 0) {
                dst_row[x] = dstRgb;
            }
        }
    }
    d_ptr->mask = temp;
    update();
}

QImage GraphicsPixmapItem::maskImage() const
{
    return d_ptr->mask;
}

void GraphicsPixmapItem::setPaintMode(GraphicsPixmapItem::Mode mode)
{
    d_ptr->mode = mode;
    setCursorPixmap();
}

GraphicsPixmapItem::Mode GraphicsPixmapItem::paintMode() const
{
    return d_ptr->mode;
}

void GraphicsPixmapItem::setPenSize(int size)
{
    d_ptr->penSize = size;
    setCursorPixmap();
    update();
}

int GraphicsPixmapItem::penSize() const
{
    return d_ptr->penSize;
}

void GraphicsPixmapItem::setOpacity(double opacity)
{
    d_ptr->opacity = opacity;
    update();
}

double GraphicsPixmapItem::opacity()
{
    return d_ptr->opacity;
}

void GraphicsPixmapItem::setMaskColor1(const QColor &color)
{
    d_ptr->color1 = color;
}

QColor GraphicsPixmapItem::maskColor1()
{
    return d_ptr->color1;
}

void GraphicsPixmapItem::setMaskColor12(const QColor &color)
{
    d_ptr->color2 = color;
}

QColor GraphicsPixmapItem::maskColor2()
{
    return d_ptr->color2;
}

void GraphicsPixmapItem::clearMask()
{
    if (!pixmap().isNull()) {
        d_ptr->mask = QImage(pixmap().size(), QImage::Format_ARGB32);
        d_ptr->mask.fill(Qt::transparent);
    }
    update();
}

void GraphicsPixmapItem::setCursorPixmap()
{
    if (d_ptr->mode == Normal) {
        unsetCursor();
        return;
    }

    double size = d_ptr->penSize * d_ptr->scaleFactor;
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(d_ptr->color1);
    pen.setWidth(1);
    painter.setPen(pen);
    d_ptr->maskPixmap = QPixmap(size, size);
    if (d_ptr->mode == MaskErase) {
        d_ptr->maskPixmap.fill(Qt::white);
        painter.setBrush(Qt::white);
    } else {
        double w = size / 2.0;
        QPainter p(&d_ptr->maskPixmap);
        p.setPen(d_ptr->color1);
        p.setBrush(d_ptr->color1);
        p.drawRect(0, 0, w, w);
        p.drawRect(w, w, w, w);
        p.setBrush(d_ptr->color2);
        p.drawRect(w, 0, w, w);
        p.drawRect(0, w, w, w);
    }
    painter.setBrush(QBrush(d_ptr->maskPixmap)); // 也可用自定义的图片
    d_ptr->maskPixmap = d_ptr->maskPixmap.scaled(d_ptr->penSize,
                                                 d_ptr->penSize,
                                                 Qt::KeepAspectRatio,
                                                 Qt::SmoothTransformation);
    painter.drawEllipse(pixmap.rect());

    QCursor cursor(pixmap);
    setCursor(cursor);
}

void GraphicsPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mousePressEvent(event);
    if (event->button() != Qt::LeftButton) {
        return;
    }
    d_ptr->lastPos = event->scenePos();
    d_ptr->currPos = event->scenePos();
}

void GraphicsPixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseMoveEvent(event);
    if (event->buttons() & Qt::LeftButton) {
        d_ptr->currPos = event->scenePos();
        if (d_ptr->mode != Normal && d_ptr->currPos != d_ptr->lastPos) {
            paintImage();
        }
    }
}

void GraphicsPixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseReleaseEvent(event);
    if (event->button() != Qt::LeftButton) {
        return;
    }
    d_ptr->lastPos = QPointF();
    d_ptr->currPos = QPointF();
}

void GraphicsPixmapItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseDoubleClickEvent(event);
    if (event->button() != Qt::LeftButton) {
        return;
    }
    d_ptr->lastPos = QPointF();
    d_ptr->currPos = QPointF();
}

void GraphicsPixmapItem::paintImage()
{
    if (d_ptr->mask.isNull()) {
        return;
    }
    QPainter p(&d_ptr->mask);
    QPen pen;
    pen.setWidth(d_ptr->penSize);
    pen.setCapStyle(Qt::RoundCap);
    //pen.setColor(d_ptr->color);
    pen.setBrush(d_ptr->maskPixmap);
    p.setPen(pen);
    if (d_ptr->mode == MaskErase) {
        p.setCompositionMode(QPainter::CompositionMode_Clear);
    }
    p.drawLine(d_ptr->lastPos, d_ptr->currPos);
    d_ptr->lastPos = d_ptr->currPos;
    update();
}

void GraphicsPixmapItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    Q_UNUSED(option)
    QStyleOptionGraphicsItem op;
    op.initFrom(widget);
    op.state = QStyle::State_None;

    QGraphicsPixmapItem::paint(painter, &op, widget);

    if (d_ptr->mask.isNull()) {
        return;
    }
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setOpacity(d_ptr->opacity);
    painter->drawImage(0, 0, d_ptr->mask);

    double factor = painter->transform().m11();
    if (qFuzzyCompare(factor, d_ptr->scaleFactor)) {
        return;
    }
    d_ptr->scaleFactor = factor;
    setCursorPixmap();
}

} // namespace Graphics

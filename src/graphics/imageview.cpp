#include "imageview.h"
#include "graphicspixmapitem.h"

#include <utils/imagecache.hpp>

#include <QtWidgets>

namespace Graphics {

class ImageView::ImageViewPrivate
{
public:
    explicit ImageViewPrivate(ImageView *q)
        : q_ptr(q)
        , menu(new QMenu)
    {
        pixmapItem = new GraphicsPixmapItem;

        // background item
        backgroundItem = new QGraphicsRectItem;
        backgroundItem->setBrush(Qt::white);
        backgroundItem->setPen(Qt::NoPen);
        backgroundItem->setVisible(showBackground);
        backgroundItem->setZValue(-1);

        // outline
        outlineItem = new QGraphicsRectItem;
        QPen outline(Qt::black, 1, Qt::DashLine);
        outline.setCosmetic(true);
        outlineItem->setPen(outline);
        outlineItem->setBrush(Qt::NoBrush);
        outlineItem->setVisible(showOutline);
        outlineItem->setZValue(1);
    }

    ~ImageViewPrivate() {}

    ImageView *q_ptr;

    GraphicsPixmapItem *pixmapItem;
    QGraphicsRectItem *backgroundItem;
    QGraphicsRectItem *outlineItem;
    bool showBackground = false;
    bool showOutline = false;
    bool showCrossLine = false;
    QString rgbInfo;
    QPointF mousePoint;
    QScopedPointer<QMenu> menu;

    const qreal scaleFactor = 1.2;

    QScopedPointer<QMovie> movie;
};

ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent)
    , d_ptr(new ImageViewPrivate(this))
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    //setDragMode(ScrollHandDrag);
    setViewportUpdateMode(SmartViewportUpdate);
    setFrameShape(QFrame::NoFrame);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
    setAcceptDrops(true);
    initScene();
    createPopMenu();
}

ImageView::ImageView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(parent)
    , d_ptr(new ImageViewPrivate(this))
{
    setScene(scene);
    setTransformationAnchor(AnchorUnderMouse);
    //setDragMode(ScrollHandDrag);
    setViewportUpdateMode(SmartViewportUpdate);
    setFrameShape(QFrame::NoFrame);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
    setAcceptDrops(true);
    initScene();
    createPopMenu();
}

ImageView::~ImageView() {}

auto ImageView::pixmap() const -> QPixmap
{
    return d_ptr->pixmapItem->pixmap();
}

auto ImageView::pixmapItem() -> GraphicsPixmapItem *
{
    return d_ptr->pixmapItem;
}

void ImageView::createScene(const QString &imageUrl)
{
    qDebug() << imageUrl;
    if (imageUrl.isEmpty()) {
        return;
    }

    QImageReader imageRender(imageUrl);
    if (!imageRender.canRead()) {
        QMessageBox::warning(this,
                             tr("WARNING"),
                             tr("Picture failed to open, Url: %1!").arg(imageUrl));
        return;
    }

    emit imageUrlChanged(imageUrl);
    setImagerReader(&imageRender);
}

void ImageView::setPixmap(const QPixmap &pixmap)
{
    if (pixmap.isNull()) {
        QMessageBox::warning(this, tr("WARNING"), tr("Pixmap is null!"));
        return;
    }

    d_ptr->pixmapItem->setCustomPixmap(pixmap);
    auto rectF = d_ptr->pixmapItem->boundingRect();
    d_ptr->backgroundItem->setRect(rectF);
    d_ptr->outlineItem->setRect(rectF);

    scene()->setSceneRect(rectF);
    resetToOriginalSize();

    if (pixmap.size().width() > width() || pixmap.size().height() > height()) {
        fitToScreen();
    }

    emit imageSizeChanged(pixmap.size());
}

void ImageView::setImagerReader(QImageReader *imageReader)
{
    if (!imageReader->supportsAnimation()) {
        d_ptr->movie.reset();
        QImage image;
        if (!Utils::ImageCache::instance()->find(imageReader->fileName(), image)) {
            QMessageBox::warning(this,
                                 tr("WARNING"),
                                 tr("Picture failed to open, Url: %1!").arg(imageReader->fileName()));
            return;
        }
        setPixmap(QPixmap::fromImage(image));
        return;
    }

    d_ptr->movie.reset(new QMovie(imageReader->fileName()));
    if (!d_ptr->movie->isValid()) {
        qWarning() << d_ptr->movie->lastError() << d_ptr->movie->lastErrorString();
    }
    connect(d_ptr->movie.data(),
            &QMovie::frameChanged,
            this,
            &ImageView::onMovieFrameChanged,
            Qt::UniqueConnection);
    d_ptr->movie->start();
}

void ImageView::setViewBackground(bool enable)
{
    d_ptr->showBackground = enable;
    d_ptr->backgroundItem->setVisible(enable);
}

void ImageView::setViewOutline(bool enable)
{
    d_ptr->showOutline = enable;
    d_ptr->outlineItem->setVisible(enable);
}

void ImageView::setViewCrossLine(bool enable)
{
    d_ptr->showCrossLine = enable;
    if (!enable) {
        scene()->update();
    }
}

void ImageView::zoomIn()
{
    doScale(d_ptr->scaleFactor);
}

void ImageView::zoomOut()
{
    doScale(1.0 / d_ptr->scaleFactor);
}

void ImageView::resetToOriginalSize()
{
    resetTransform();
    emitScaleFactor();
}

void ImageView::fitToScreen()
{
    fitInView(d_ptr->pixmapItem, Qt::KeepAspectRatio);
    emitScaleFactor();
}

void ImageView::rotateNinetieth()
{
    rotate(90);
}

void ImageView::anti_rotateNinetieth()
{
    rotate(-90);
}

void ImageView::onMovieFrameChanged(int frameNumber)
{
    Q_UNUSED(frameNumber)
    //qDebug() << frameNumber;
    setPixmap(d_ptr->movie->currentPixmap());
}

void ImageView::drawBackground(QPainter *p, const QRectF &)
{
    p->save();
    p->resetTransform();
    p->setRenderHint(QPainter::SmoothPixmapTransform, false);
    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    p->restore();
}

void ImageView::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawForeground(painter, rect);
    painter->save();
    painter->resetTransform();

    if (d_ptr->showCrossLine && d_ptr->pixmapItem) {
        drawInfo(painter);
        drawCrossLine(painter);
    }

    painter->restore();
}

void ImageView::wheelEvent(QWheelEvent *event)
{
    QGraphicsView::wheelEvent(event);
    if (d_ptr->pixmapItem->pixmap().isNull()) {
        return;
    }
    qreal factor = qPow(d_ptr->scaleFactor, event->angleDelta().y() / 240.0);
    doScale(factor);
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    if (!d_ptr->showCrossLine || !d_ptr->pixmapItem) {
        return;
    }
    QPointF pointF = mapToScene(event->pos());
    d_ptr->mousePoint = event->pos();
    if (!d_ptr->pixmapItem->contains(pointF)) {
        return;
    }
    QRgb rgb = d_ptr->pixmapItem->pixmap().toImage().pixel(pointF.toPoint());
    d_ptr->rgbInfo = QString("( %1, %2 ) | %3 %4 %5")
                         .arg(QString::number(pointF.x()),
                              QString::number(pointF.y()),
                              QString::number(qRed(rgb)),
                              QString::number(qGreen(rgb)),
                              QString::number(qBlue(rgb)));
    scene()->update();
}

void ImageView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);
    fitToScreen();
}

void ImageView::dragEnterEvent(QDragEnterEvent *event)
{
    QGraphicsView::dragEnterEvent(event);
    event->acceptProposedAction();
}

void ImageView::dragMoveEvent(QDragMoveEvent *event)
{
    QGraphicsView::dragMoveEvent(event);
    event->acceptProposedAction();
}

void ImageView::dropEvent(QDropEvent *event)
{
    QGraphicsView::dropEvent(event);
    const auto urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }
    createScene(urls.first().toLocalFile());
}

void ImageView::contextMenuEvent(QContextMenuEvent *event)
{
    d_ptr->menu->exec(event->globalPos());
}

void ImageView::initScene()
{
    // Prepare background check-board pattern
    QPixmap tilePixmap(64, 64);
    tilePixmap.fill(Qt::white);
    QPainter tilePainter(&tilePixmap);
    QColor color(220, 220, 220);
    tilePainter.fillRect(0, 0, 0x20, 0x20, color);
    tilePainter.fillRect(0x20, 0x20, 0x20, 0x20, color);
    tilePainter.end();

    setBackgroundBrush(tilePixmap);

    scene()->addItem(d_ptr->backgroundItem);
    scene()->addItem(d_ptr->pixmapItem);
    scene()->addItem(d_ptr->outlineItem);
}

void ImageView::createPopMenu()
{
    d_ptr->menu->addAction(tr("Original Size"), this, &ImageView::resetToOriginalSize);
    d_ptr->menu->addAction(tr("Adapt To Screen"), this, &ImageView::fitToScreen);
    d_ptr->menu->addAction(tr("Rotate 90 Clockwise"), this, &ImageView::rotateNinetieth);
    d_ptr->menu->addAction(tr("Rotate 90 Counterclockwise"), this, &ImageView::anti_rotateNinetieth);
    d_ptr->menu->addSeparator();

    QAction *showBackgroundAction = new QAction(tr("Show Background"), this);
    showBackgroundAction->setCheckable(true);
    connect(showBackgroundAction, &QAction::triggered, this, &ImageView::setViewBackground);
    QAction *showOutlineAction = new QAction(tr("Show Outline"), this);
    showOutlineAction->setCheckable(true);
    connect(showOutlineAction, &QAction::triggered, this, &ImageView::setViewOutline);
    QAction *showCrossLineAction = new QAction(tr("Show CrossLine"), this);
    showCrossLineAction->setCheckable(true);
    connect(showCrossLineAction, &QAction::triggered, this, &ImageView::setViewCrossLine);
    d_ptr->menu->addAction(showBackgroundAction);
    d_ptr->menu->addAction(showOutlineAction);
    d_ptr->menu->addAction(showCrossLineAction);
}

auto ImageView::textRect(const Qt::Corner pos, const QFontMetrics &metrics, const QString &text)
    -> QRect
{
    int startX = 1;
    int startY = 1;
    int rectWidth = metrics.horizontalAdvance(text) + 10;
    int rectHeight = metrics.lineSpacing() + 5;

    switch (pos) {
    case Qt::TopLeftCorner: break;
    case Qt::BottomLeftCorner: startY = height() - rectHeight; break;
    case Qt::TopRightCorner: startX = width() - rectWidth; break;
    case Qt::BottomRightCorner:
        startX = width() - rectWidth;
        startY = height() - rectHeight;
        break;
    }
    return QRect(startX, startY, rectWidth, rectHeight);
}

void ImageView::drawInfo(QPainter *painter)
{
    if (d_ptr->rgbInfo.isEmpty()) {
        return;
    }
    QFont font;
    font.setPixelSize(height() / 30);
    painter->setFont(font);
    QFontMetrics metrics(font);
    int marginX = 5;
    int marginY = metrics.leading() + metrics.ascent() + 2;
    QRect rect = textRect(Qt::TopLeftCorner, metrics, d_ptr->rgbInfo);
    QPoint textPos = QPoint(rect.x() + marginX, rect.y() + marginY);

    painter->setPen(Qt::NoPen);
    QColor bgColor(47, 53, 65);
    bgColor.setAlpha(200);
    painter->setBrush(bgColor);
    painter->drawRect(rect);
    painter->setPen(QColor(83, 209, 255));
    painter->drawText(textPos, d_ptr->rgbInfo);
}

void ImageView::drawCrossLine(QPainter *painter)
{
    QPen pen = painter->pen();
    pen.setColor(QColor(0, 230, 230));
    pen.setWidthF(0.8);
    painter->setPen(pen);

    int w = rect().width();
    int h = rect().height();
    painter->drawLine(QPointF(0, d_ptr->mousePoint.y()), QPointF(w, d_ptr->mousePoint.y()));
    painter->drawLine(QPointF(d_ptr->mousePoint.x(), 0), QPointF(d_ptr->mousePoint.x(), h));
}

void ImageView::emitScaleFactor()
{
    // get scale factor directly from the transform matrix
    qreal factor = transform().m11() * devicePixelRatioF();
    emit scaleFactorChanged(factor);
}

void ImageView::doScale(qreal factor)
{
    //    qDebug() << factor;
    //    qreal currentScale = transform().m11();
    //    qreal newScale = currentScale * factor;
    //    qreal actualFactor = factor;
    //    // cap to 0.001 - 1000
    //    if (newScale > 1000) {
    //        actualFactor = 1000.0 / currentScale;
    //    } else if (newScale < 0.001) {
    //        actualFactor = 0.001 / currentScale;
    //    }

    //    qInfo() << actualFactor;
    scale(factor, factor);
    emitScaleFactor();
    d_ptr->pixmapItem->setTransformationMode(transform().m11() < 1 ? Qt::SmoothTransformation
                                                                   : Qt::FastTransformation);
}

void ImageView::reset()
{
    scene()->clear();
    resetTransform();
}

} // namespace Graphics

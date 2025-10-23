#pragma once

#include "graphics_global.h"

#include <QGraphicsView>

namespace Graphics {

class GraphicsPixmapItem;
class GRAPHICS_EXPORT GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = nullptr);
    explicit GraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~GraphicsView() override;

    [[nodiscard]] auto pixmap() const -> QPixmap;
    auto pixmapItem() -> GraphicsPixmapItem *;

public slots:
    void createScene(const QString &imageUrlChanged);
    void setPixmap(const QPixmap &pixmap);
    void setImagerReader(QImageReader &imageReader);

    void setViewBackground(bool enable);
    void setViewOutline(bool enable);
    void setViewCrossLine(bool enable);
    void zoomIn();
    void zoomOut();
    void resetToOriginalSize();
    void fitToScreen();

    void rotateNinetieth();
    void anti_rotateNinetieth();

signals:
    void scaleFactorChanged(qreal factor);
    void imageSizeChanged(const QSize &size);
    void imageUrlChanged(const QString &);

private slots:
    void onMovieFrameChanged(int frameNumber);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void initializeView();
    void initScene();
    void createPopMenu();
    auto textRect(const Qt::Corner pos, const QFontMetrics &metrics, const QString &text) -> QRect;
    void drawInfo(QPainter *painter);
    void drawCrossLine(QPainter *painter);
    void emitScaleFactor();
    void doScale(qreal factor);
    void reset();

    class ImageViewPrivate;
    QScopedPointer<ImageViewPrivate> d_ptr;
};

} // namespace Graphics

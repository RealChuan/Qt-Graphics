#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include "viewer.hpp"

class ImageViewer : public Viewer
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer() override;

    auto setImage(const QFileInfo &info, const QImage &image, const qint64 taskCount) -> bool override;

signals:
    void imageReady(const QImage &);

private slots:
    void onOpenImage();
    void onMaskImage();
    void onRoundImage();

    void onScaleFactorChanged(qreal factor);
    void onImageSizeChanged(const QSize &size);
    void onImageChanged(const QString &);
    void onChangedImage(int);
    void onImageLoaded(const QFileInfo &fileInfo, const QImage &image);

    void onFormatChecked(bool);
    void onFormatChanged(const QString &);

private:
    void startImageLoadThread(const QString &url);
    void clearThumbnail();
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class ImageViewerPrivate;
    QScopedPointer<ImageViewerPrivate> d_ptr;
};

#endif // IMAGEVIEWER_H

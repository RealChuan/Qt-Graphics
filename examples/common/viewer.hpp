#pragma once

#include "imagelistmodel.h"

#include <QRunnable>

class QGroupBox;
class QLabel;
class QToolButton;

class Viewer : public QWidget
{
    Q_OBJECT
public:
    explicit Viewer(QWidget *parent = nullptr);
    ~Viewer() override;

    virtual auto setImage(const QFileInfo &info, const QImage &image, const qint64 taskCount)
        -> bool;

protected slots:
    void onScaleFactorChanged(qreal factor);
    void onImageSizeChanged(const QSize &size);
    void onImageChanged(const QString &url);

protected:
    QString openImage();
    virtual void startImageLoadThread(const QString &url);
    virtual void appendThumbnail(const QFileInfo &fileInfo, const QImage &image);
    virtual void clearThumbnail();

    QToolButton *m_openButton;

    QGroupBox *m_infoBox;
    QLabel *m_urlLabel;
    QLabel *m_fileSizeLabel;
    QLabel *m_sizeLabel;
    QLabel *m_scaleLabel;
    ThumbnailList m_thumbnailList;
    ImageListView *m_imageListView;

    std::atomic_llong m_taskCount = 1;

private:
    void setupUI();
};

class ImageLoadRunnable : public QRunnable
{
public:
    explicit ImageLoadRunnable(const QString &fileUrl, Viewer *view, qint64 taskCount);
    ~ImageLoadRunnable() override;

    static void terminateAll();

protected:
    void run() override;

private:
    class ImageLoadRunnablePrivate;
    QScopedPointer<ImageLoadRunnablePrivate> d_ptr;
};

#pragma once

#include <examples/common/viewer.hpp>

class RhiViewer : public Viewer
{
    Q_OBJECT
public:
    explicit RhiViewer(QWidget *parent = nullptr);
    ~RhiViewer() override;

    auto setImage(const QFileInfo &info,
                  const QImage &image,
                  const qint64 taskCount) -> bool override;

signals:
    void imageReady(const QImage &);

private slots:
    void onOpenImage();
    void onBackendChanged(const QString &name);

    void onScaleFactorChanged(qreal factor);
    void onImageSizeChanged(const QSize &size);
    void onImageChanged(const QString &url);
    void onChangedImage(int index);
    void onImageLoaded(const QFileInfo &fileInfo, const QImage &image);

private:
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();
    void startImageLoadThread(const QString &url);
    void clearThumbnail();

    class RhiViewerPrivate;
    QScopedPointer<RhiViewerPrivate> d_ptr;
};

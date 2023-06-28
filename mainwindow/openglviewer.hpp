#pragma once

#include "viewer.hpp"

class OpenglViewer : public Viewer
{
    Q_OBJECT
public:
    explicit OpenglViewer(QWidget *parent = nullptr);
    ~OpenglViewer() override;

    auto setImage(const QFileInfo &info, const QImage &image, const qint64 taskCount) -> bool override;

signals:
    void imageReady(const QImage &);

private slots:
    void onOpenImage();

    void onScaleFactorChanged(qreal factor);
    void onImageSizeChanged(const QSize &size);
    void onImageChanged(const QString &);
    void onChangedImage(int);
    void onImageLoaded(const QFileInfo &fileInfo, const QImage &image);

private:
    void startImageLoadThread(const QString &url);
    void clearThumbnail();
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class OpenglViewerPrivate;
    QScopedPointer<OpenglViewerPrivate> d_ptr;
};

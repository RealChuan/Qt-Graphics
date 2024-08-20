#ifndef VULKANVIEWER_HPP
#define VULKANVIEWER_HPP

#include <examples/common/viewer.hpp>

class VulkanViewer : public Viewer
{
    Q_OBJECT
public:
    explicit VulkanViewer(QWidget *parent = nullptr);
    ~VulkanViewer() override;

    auto setImage(const QFileInfo &info,
                  const QImage &image,
                  const qint64 taskCount) -> bool override;

signals:
    void imageReady(const QImage &);

private slots:
    void onOpenImage();

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

    class VulkanViewerPrivate;
    QScopedPointer<VulkanViewerPrivate> d_ptr;
};

#endif // VULKANVIEWER_HPP

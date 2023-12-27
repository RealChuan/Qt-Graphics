#ifndef VULKANVIEW_HPP
#define VULKANVIEW_HPP

#include "gpugraphics_global.hpp"

#include <QVulkanWindow>

namespace GpuGraphics {

class VulkanRenderer;

class GPUAPHICS VulkanView : public QVulkanWindow
{
    Q_OBJECT
public:
    explicit VulkanView(QWindow *parent = nullptr);
    ~VulkanView() override;

    auto createRenderer() -> QVulkanWindowRenderer * override;

    void setImageUrl(const QString &imageUrl);

signals:
    void scaleFactorChanged(qreal factor);
    void imageSizeChanged(const QSize &size);
    void imageUrlChanged(const QString &);

private slots:
    void emitScaleFactor();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void createPopMenu();

    class VulkanViewPrivate;
    QScopedPointer<VulkanViewPrivate> d_ptr;
};

} // namespace GpuGraphics

#endif // VULKANVIEW_HPP

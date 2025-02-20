#ifndef VULKANVIEWER_HPP
#define VULKANVIEWER_HPP

#include <examples/common/viewer.hpp>

class VulkanViewer : public Viewer
{
    Q_OBJECT
public:
    explicit VulkanViewer(QWidget *parent = nullptr);
    ~VulkanViewer() override;

    static bool isSupported();

private slots:
    void onOpenImage();
    void onChangedImage(int index);

private:
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class VulkanViewerPrivate;
    QScopedPointer<VulkanViewerPrivate> d_ptr;
};

#endif // VULKANVIEWER_HPP

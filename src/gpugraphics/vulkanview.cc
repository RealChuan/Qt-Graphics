#include "vulkanview.hpp"
#include "vulkanrenderer.hpp"

#include <QLoggingCategory>
#include <QtWidgets>

namespace GpuGraphics {

bool isVulkanSupported()
{
    QVulkanInstance inst;
    if (inst.create()) {
        inst.destroy();
        return true;
    }
    qWarning() << "Vulkan is not supported, error code:" << inst.errorCode();
    return false;
}

class VulkanView::VulkanViewPrivate
{
public:
    explicit VulkanViewPrivate(VulkanView *q)
        : q_ptr(q)
    {
        QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));
#ifndef Q_OS_ANDROID
        inst.setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");
#else
        inst.setLayers(QByteArrayList() << "VK_LAYER_GOOGLE_threading"
                                        << "VK_LAYER_LUNARG_parameter_validation"
                                        << "VK_LAYER_LUNARG_object_tracker"
                                        << "VK_LAYER_LUNARG_core_validation"
                                        << "VK_LAYER_LUNARG_image"
                                        << "VK_LAYER_LUNARG_swapchain"
                                        << "VK_LAYER_GOOGLE_unique_objects");
#endif

        if (!inst.create()) {
            qFatal("Failed to create Vulkan instance: %d", inst.errorCode());
        }
        qInfo() << "Vulkan Version: " << inst.apiVersion();
        q_ptr->setVulkanInstance(&inst);

        menuPtr.reset(new QMenu);
    }
    ~VulkanViewPrivate() = default;

    VulkanView *q_ptr;

    QVulkanInstance inst;
    VulkanRenderer *vulkanRenderer;
    const qreal scaleFactor = 1.2;

    QScopedPointer<QMenu> menuPtr;
};

VulkanView::VulkanView(QWindow *parent)
    : QVulkanWindow(parent)
    , d_ptr(new VulkanViewPrivate(this))
{
    createPopMenu();
}

VulkanView::~VulkanView() = default;

auto VulkanView::createRenderer() -> QVulkanWindowRenderer *
{
    d_ptr->vulkanRenderer = new VulkanRenderer(this);
    return d_ptr->vulkanRenderer;
}

void VulkanView::setImageUrl(const QString &imageUrl)
{
    auto size = d_ptr->vulkanRenderer->setImageUrl(imageUrl);
    if (!size.isValid()) {
        return;
    }
    emit imageSizeChanged(size);
    emit imageUrlChanged(imageUrl);
    emitScaleFactor();
}

void VulkanView::resizeEvent(QResizeEvent *event)
{
    QVulkanWindow::resizeEvent(event);
    QMetaObject::invokeMethod(this, &VulkanView::emitScaleFactor, Qt::QueuedConnection);
}

void VulkanView::wheelEvent(QWheelEvent *event)
{
    QVulkanWindow::wheelEvent(event);
    qreal factor = qPow(d_ptr->scaleFactor, event->angleDelta().y() / 240.0);
    d_ptr->vulkanRenderer->scale(factor, factor);
    emitScaleFactor();
}

void VulkanView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QVulkanWindow::mouseDoubleClickEvent(event);
    d_ptr->vulkanRenderer->fitToScreen();
    emitScaleFactor();
}

void VulkanView::mousePressEvent(QMouseEvent *event)
{
    QVulkanWindow::mousePressEvent(event);
    if (event->button() != Qt::RightButton) {
        return;
    }
    d_ptr->menuPtr->exec(event->globalPosition().toPoint());
}

void VulkanView::createPopMenu()
{
    d_ptr->menuPtr->clear();
    d_ptr->menuPtr->addAction(tr("Reset to original size"), this, [this] {
        d_ptr->vulkanRenderer->resetToOriginalSize();
        emitScaleFactor();
    });
    d_ptr->menuPtr->addAction(tr("Fit to screen"), this, [this] {
        d_ptr->vulkanRenderer->fitToScreen();
        emitScaleFactor();
    });
    d_ptr->menuPtr->addAction(tr("Rotate 90"), this, [this] {
        d_ptr->vulkanRenderer->rotateNinetieth();
    });
    d_ptr->menuPtr->addAction(tr("Anti rotate 90"), this, [this] {
        d_ptr->vulkanRenderer->anti_rotateNinetieth();
    });
}

void VulkanView::emitScaleFactor()
{
    emit scaleFactorChanged(d_ptr->vulkanRenderer->scaleFactor());
}

} // namespace GpuGraphics

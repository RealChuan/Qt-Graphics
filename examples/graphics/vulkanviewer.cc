#include "vulkanviewer.hpp"

#include <examples/common/imagelistmodel.h>
#include <gpugraphics/vulkanrenderer.hpp>
#include <gpugraphics/vulkanview.hpp>
#include <utils/utils.hpp>

#include <QtWidgets>

class VulkanViewer::VulkanViewerPrivate
{
public:
    explicit VulkanViewerPrivate(VulkanViewer *q)
        : q_ptr(q)
        , vulkanViewPtr(new GpuGraphics::VulkanView)
    {}

    VulkanViewer *q_ptr;

    QScopedPointer<GpuGraphics::VulkanView> vulkanViewPtr;
};

VulkanViewer::VulkanViewer(QWidget *parent)
    : Viewer(parent)
    , d_ptr(new VulkanViewerPrivate(this))
{
    setupUI();
    buildConnect();
    setEnableJumpToMultiPage(true);
}

VulkanViewer::~VulkanViewer()
{
    clearThumbnail();
}

bool VulkanViewer::isSupported()
{
    static bool ret = GpuGraphics::isVulkanSupported();
    return ret;
}

void VulkanViewer::onOpenImage()
{
    const auto filename = openImage();
    if (filename.isEmpty()) {
        return;
    }
    d_ptr->vulkanViewPtr->setImageUrl(filename);
}

void VulkanViewer::onChangedImage(int index)
{
    if (index < 0 || index >= m_thumbnailList.size()) {
        return;
    }
    const auto data = m_thumbnailList.at(index);
    d_ptr->vulkanViewPtr->setImageUrl(data.fileInfo().absoluteFilePath());
}

void VulkanViewer::setupUI()
{
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(QWidget::createWindowContainer(d_ptr->vulkanViewPtr.data()));
    splitter->addWidget(toolWidget());
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({INT_MAX, 1});

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->addWidget(splitter);
    layout->addWidget(m_imageListView);
}

auto VulkanViewer::toolWidget() -> QWidget *
{
    auto *widget = new QWidget(this);
    widget->setMaximumWidth(300);
    auto *rightLayout = new QVBoxLayout(widget);
    rightLayout->addWidget(m_openButton);
    rightLayout->addWidget(m_infoBox);
    rightLayout->addWidget(m_jumpToMultiPageButton);
    rightLayout->addStretch();

    return widget;
}

void VulkanViewer::buildConnect()
{
    connect(m_openButton, &QPushButton::clicked, this, &VulkanViewer::onOpenImage);

    connect(d_ptr->vulkanViewPtr.data(),
            &GpuGraphics::VulkanView::scaleFactorChanged,
            this,
            &VulkanViewer::onScaleFactorChanged);
    connect(d_ptr->vulkanViewPtr.data(),
            &GpuGraphics::VulkanView::imageSizeChanged,
            this,
            &VulkanViewer::onImageSizeChanged);
    connect(d_ptr->vulkanViewPtr.data(),
            &GpuGraphics::VulkanView::imageUrlChanged,
            this,
            &VulkanViewer::onImageChanged);
    connect(m_imageListView, &ImageListView::changeItem, this, &VulkanViewer::onChangedImage);
}

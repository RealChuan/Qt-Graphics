#include "openglviewer.hpp"

#include <examples/common/imagelistmodel.h>
#include <gpugraphics/openglview.hpp>
#include <utils/utils.h>

#include <QtWidgets>

class OpenglViewer::OpenglViewerPrivate
{
public:
    explicit OpenglViewerPrivate(OpenglViewer *q)
        : q_ptr(q)
    {
        openglView = new GpuGraphics::OpenglView(q_ptr);
    }
    ~OpenglViewerPrivate() {}

    OpenglViewer *q_ptr;

    GpuGraphics::OpenglView *openglView;
};

OpenglViewer::OpenglViewer(QWidget *parent)
    : Viewer(parent)
    , d_ptr(new OpenglViewerPrivate(this))
{
    setupUI();
    buildConnect();
}

OpenglViewer::~OpenglViewer() {}

void OpenglViewer::onOpenImage()
{
    const auto filename = openImage();
    if (filename.isEmpty()) {
        return;
    }
    d_ptr->openglView->setImageUrl(filename);
}

void OpenglViewer::onChangedImage(int index)
{
    if (index < 0 || index >= m_thumbnailList.size()) {
        return;
    }
    const auto data = m_thumbnailList.at(index);
    d_ptr->openglView->setImageUrl(data.fileInfo().absoluteFilePath());
}

void OpenglViewer::setupUI()
{
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(d_ptr->openglView);
    splitter->addWidget(toolWidget());
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({INT_MAX, 1});

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->addWidget(splitter);
    layout->addWidget(m_imageListView);
}

QWidget *OpenglViewer::toolWidget()
{
    auto *widget = new QWidget(this);
    widget->setMaximumWidth(300);
    auto *rightLayout = new QVBoxLayout(widget);
    rightLayout->addWidget(m_openButton);
    rightLayout->addWidget(m_infoBox);
    rightLayout->addStretch();

    return widget;
}

void OpenglViewer::buildConnect()
{
    connect(m_openButton, &QPushButton::clicked, this, &OpenglViewer::onOpenImage);

    connect(d_ptr->openglView,
            &GpuGraphics::OpenglView::scaleFactorChanged,
            this,
            &OpenglViewer::onScaleFactorChanged);
    connect(d_ptr->openglView,
            &GpuGraphics::OpenglView::imageSizeChanged,
            this,
            &OpenglViewer::onImageSizeChanged);
    connect(d_ptr->openglView,
            &GpuGraphics::OpenglView::imageUrlChanged,
            this,
            &OpenglViewer::onImageChanged);
    connect(m_imageListView, &ImageListView::changeItem, this, &OpenglViewer::onChangedImage);
}

#include "rhiviewer.hpp"

#include <examples/common/imagelistmodel.h>
#include <gpugraphics/rhiview.hpp>
#include <utils/utils.h>

#include <QtWidgets>

class RhiViewer::RhiViewerPrivate
{
public:
    explicit RhiViewerPrivate(RhiViewer *q)
        : q_ptr(q)
    {
        rhiView = new GpuGraphics::RhiView(q_ptr);

        backendNameLabel = new QLabel(q_ptr);
        backendNameLabel->setAlignment(Qt::AlignCenter);
    }

    RhiViewer *q_ptr;

    GpuGraphics::RhiView *rhiView;

    QLabel *backendNameLabel;
};

RhiViewer::RhiViewer(QWidget *parent)
    : Viewer{parent}
    , d_ptr(new RhiViewerPrivate(this))
{
    setupUI();
    buildConnect();
}

RhiViewer::~RhiViewer() {}

void RhiViewer::onOpenImage()
{
    const auto filename = openImage();
    if (filename.isEmpty()) {
        return;
    }
    d_ptr->rhiView->setImageUrl(filename);
}

void RhiViewer::onBackendChanged(const QString &name)
{
    auto text = tr("Using QRhi on:  %1").arg(name);
    d_ptr->backendNameLabel->setText(text);
    d_ptr->backendNameLabel->setToolTip(text);
}

void RhiViewer::onChangedImage(int index)
{
    if (index < 0 || index >= m_thumbnailList.size()) {
        return;
    }
    const auto data = m_thumbnailList.at(index);
    d_ptr->rhiView->setImageUrl(data.fileInfo().absoluteFilePath());
}

void RhiViewer::setupUI()
{
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(d_ptr->rhiView);
    splitter->addWidget(toolWidget());
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({INT_MAX, 1});

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->addWidget(splitter);
    layout->addWidget(m_imageListView);
}

QWidget *RhiViewer::toolWidget()
{
    auto *widget = new QWidget(this);
    widget->setMaximumWidth(300);
    auto *rightLayout = new QVBoxLayout(widget);
    rightLayout->setSpacing(15);
    rightLayout->addWidget(m_openButton);
    rightLayout->addWidget(d_ptr->backendNameLabel);
    rightLayout->addWidget(m_infoBox);
    rightLayout->addStretch();

    return widget;
}

void RhiViewer::buildConnect()
{
    connect(m_openButton, &QPushButton::clicked, this, &RhiViewer::onOpenImage);

    connect(d_ptr->rhiView, &GpuGraphics::RhiView::rhiChanged, this, &RhiViewer::onBackendChanged);
    connect(d_ptr->rhiView,
            &GpuGraphics::RhiView::scaleFactorChanged,
            this,
            &RhiViewer::onScaleFactorChanged);
    connect(d_ptr->rhiView,
            &GpuGraphics::RhiView::imageSizeChanged,
            this,
            &RhiViewer::onImageSizeChanged);
    connect(d_ptr->rhiView,
            &GpuGraphics::RhiView::imageUrlChanged,
            this,
            &RhiViewer::onImageChanged);
    connect(m_imageListView, &ImageListView::changeItem, this, &RhiViewer::onChangedImage);
}

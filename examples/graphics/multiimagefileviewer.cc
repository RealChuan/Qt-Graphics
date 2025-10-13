#include "multiimagefileviewer.hpp"

#include <graphics/imageview.h>
#include <utils/utils.hpp>

#include <QtWidgets>

class MultiImageFileViewer::MultiImageFileViewerPrivate
{
public:
    explicit MultiImageFileViewerPrivate(MultiImageFileViewer *q)
        : q_ptr(q)
    {
        imageView = new Graphics::ImageView(q_ptr);
    }
    ~MultiImageFileViewerPrivate() {}

    MultiImageFileViewer *q_ptr;

    Graphics::ImageView *imageView;
};

MultiImageFileViewer::MultiImageFileViewer(QWidget *parent)
    : Viewer(parent)
    , d_ptr(new MultiImageFileViewerPrivate(this))
{
    setupUI();
    buildConnect();
}

MultiImageFileViewer::~MultiImageFileViewer() {}

void MultiImageFileViewer::onOpenImage()
{
    const QString imageFilters(tr("Images (*.ico *.gif)"));
    const auto path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                          .value(0, QDir::homePath());
    const auto filename = QFileDialog::getOpenFileName(this, tr("Open Image"), path, imageFilters);
    if (filename.isEmpty()) {
        return;
    }
    d_ptr->imageView->createScene(filename);
}

void MultiImageFileViewer::onChangedImage(int index)
{
    if (index < 0 || index >= m_thumbnailList.size()) {
        return;
    }
    const auto data = m_thumbnailList.at(index);
    d_ptr->imageView->setPixmap(QPixmap::fromImage(data.image()));
}

void MultiImageFileViewer::onImageUrlChanged(const QString &url)
{
    if (url.isEmpty()) {
        return;
    }
    m_urlLabel->setText(url);
    m_urlLabel->setToolTip(url);

    auto sizeText = Utils::formatBytes(QFile(url).size());
    m_fileSizeLabel->setText(sizeText);
    m_fileSizeLabel->setToolTip(sizeText);

    m_openButton->setEnabled(false);

    clearThumbnail();
    auto images = Utils::asynchronous<QList<QImage>>([url]() { return Utils::readImages(url); });
    for (const auto &image : std::as_const(images)) {
        appendThumbnail({{}, image});
    }

    m_openButton->setEnabled(true);
}

void MultiImageFileViewer::setupUI()
{
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(d_ptr->imageView);
    splitter->addWidget(toolWidget());
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({INT_MAX, 1});

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->addWidget(splitter);
    layout->addWidget(m_imageListView);
}

auto MultiImageFileViewer::toolWidget() -> QWidget *
{
    auto *widget = new QWidget(this);
    widget->setMaximumWidth(300);
    auto *rightLayout = new QVBoxLayout(widget);
    rightLayout->addWidget(m_openButton);
    rightLayout->addWidget(m_infoBox);
    rightLayout->addStretch();

    return widget;
}

void MultiImageFileViewer::buildConnect()
{
    connect(m_openButton, &QPushButton::clicked, this, &MultiImageFileViewer::onOpenImage);
    connect(d_ptr->imageView,
            &Graphics::ImageView::scaleFactorChanged,
            this,
            &MultiImageFileViewer::onScaleFactorChanged);
    connect(d_ptr->imageView,
            &Graphics::ImageView::imageSizeChanged,
            this,
            &MultiImageFileViewer::onImageSizeChanged);
    connect(d_ptr->imageView,
            &Graphics::ImageView::imageUrlChanged,
            this,
            &MultiImageFileViewer::onImageUrlChanged);
    connect(m_imageListView,
            &ImageListView::changeItem,
            this,
            &MultiImageFileViewer::onChangedImage);
}

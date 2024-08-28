#include "vulkanviewer.hpp"

#include <examples/common/imagelistmodel.h>
#include <gpugraphics/vulkanrenderer.hpp>
#include <gpugraphics/vulkanview.hpp>
#include <utils/utils.h>

#include <QtWidgets>

class VulkanViewer::VulkanViewerPrivate
{
public:
    explicit VulkanViewerPrivate(VulkanViewer *q)
        : q_ptr(q)
        , vulkanViewPtr(new GpuGraphics::VulkanView)
    {
        urlLabel = new QLabel("-", q_ptr);
        urlLabel->setWordWrap(true);
        sizeLabel = new QLabel("-", q_ptr);
        fileSizeLabel = new QLabel("-", q_ptr);
        scaleLabel = new QLabel("-", q_ptr);
        imageListView = new ImageListView(q_ptr);
        imageListView->setFixedHeight(120);
    }

    VulkanViewer *q_ptr;

    QScopedPointer<GpuGraphics::VulkanView> vulkanViewPtr;

    QLabel *urlLabel;
    QLabel *fileSizeLabel;
    QLabel *sizeLabel;
    QLabel *scaleLabel;
    ImageInfoList imageInfoList;
    ImageListView *imageListView;

    QAtomicInteger<qint64> taskCount = 0;
};

VulkanViewer::VulkanViewer(QWidget *parent)
    : Viewer(parent)
    , d_ptr(new VulkanViewerPrivate(this))
{
    setupUI();
    buildConnect();
}

VulkanViewer::~VulkanViewer()
{
    clearThumbnail();
}

auto VulkanViewer::setImage(const QFileInfo &info,
                            const QImage &image,
                            const qint64 taskCount) -> bool
{
    if (taskCount != d_ptr->taskCount.loadRelaxed()) {
        return false;
    }
    QMetaObject::invokeMethod(this, [=] { onImageLoaded(info, image); }, Qt::QueuedConnection);
    return true;
}

void VulkanViewer::onOpenImage()
{
    QString imageFilters(tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.svg *.tiff *.webp *.icns "
                            "*.bitmap *.graymap *.pixmap *.tga *.xbitmap *.xpixmap)"));
    //qDebug() << imageFilters;
    const auto path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                          .value(0, QDir::homePath());
    const auto filename = QFileDialog::getOpenFileName(this, tr("Open Image"), path, imageFilters);
    if (filename.isEmpty()) {
        return;
    }

    d_ptr->vulkanViewPtr->setImageUrl(filename);
}

void VulkanViewer::onScaleFactorChanged(qreal factor)
{
    const auto info = QString::number(factor * 100, 'f', 2) + QLatin1Char('%');
    d_ptr->scaleLabel->setText(info);
}

void VulkanViewer::onImageSizeChanged(const QSize &size)
{
    QString imageSizeText;
    if (size.isValid()) {
        imageSizeText = QString::fromLatin1("%1x%2").arg(size.width()).arg(size.height());
    }
    d_ptr->sizeLabel->setText(imageSizeText);
}

void VulkanViewer::onImageChanged(const QString &url)
{
    d_ptr->urlLabel->setText(url);
    d_ptr->fileSizeLabel->setText(Utils::convertBytesToString(QFile(url).size()));

    for (const ImageInfo &image : std::as_const(d_ptr->imageInfoList)) {
        if (image.fileInfo().absoluteFilePath() == url) {
            return;
        }
    }
    clearThumbnail();
    startImageLoadThread(url);
}

void VulkanViewer::onChangedImage(int index)
{
    if (index < 0 || index >= d_ptr->imageInfoList.size()) {
        return;
    }
    const ImageInfo &info = d_ptr->imageInfoList.at(index);
    d_ptr->vulkanViewPtr->setImageUrl(info.fileInfo().absoluteFilePath());
}

void VulkanViewer::onImageLoaded(const QFileInfo &fileInfo, const QImage &image)
{
    if (image.isNull()) {
        return;
    }
    d_ptr->imageInfoList.append(ImageInfo(fileInfo, image));
    d_ptr->imageListView->setImageInfoList(d_ptr->imageInfoList);
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
    layout->addWidget(d_ptr->imageListView);
}

auto VulkanViewer::toolWidget() -> QWidget *
{
    auto *openImageButton = new QPushButton(tr("Open Picture"), this);
    connect(openImageButton, &QPushButton::clicked, this, &VulkanViewer::onOpenImage);

    auto *infoBox = new QGroupBox(tr("Image Information"), this);
    auto *gridLayout = new QGridLayout(infoBox);
    gridLayout->addWidget(new QLabel(tr("Url: "), this), 0, 0, 1, 1);
    gridLayout->addWidget(d_ptr->urlLabel, 0, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("File Size: "), this), 1, 0, 1, 1);
    gridLayout->addWidget(d_ptr->fileSizeLabel, 1, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Image Size: "), this), 2, 0, 1, 1);
    gridLayout->addWidget(d_ptr->sizeLabel, 2, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Scaling Ratio:"), this), 3, 0, 1, 1);
    gridLayout->addWidget(d_ptr->scaleLabel, 3, 1, 1, 1);

    auto *widget = new QWidget(this);
    widget->setMaximumWidth(300);
    auto *rightLayout = new QVBoxLayout(widget);
    rightLayout->addWidget(openImageButton);
    rightLayout->addWidget(infoBox);
    rightLayout->addStretch();

    return widget;
}

void VulkanViewer::buildConnect()
{
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
    connect(d_ptr->imageListView, &ImageListView::changeItem, this, &VulkanViewer::onChangedImage);
}

void VulkanViewer::startImageLoadThread(const QString &url)
{
    d_ptr->taskCount.ref();
    QThreadPool::globalInstance()->start(
        new ImageLoadRunnable(url, this, d_ptr->taskCount.loadAcquire()));
}

void VulkanViewer::clearThumbnail()
{
    if (d_ptr->imageInfoList.isEmpty()) {
        return;
    }
    d_ptr->imageInfoList.clear();
    d_ptr->imageListView->setImageInfoList(d_ptr->imageInfoList);
}

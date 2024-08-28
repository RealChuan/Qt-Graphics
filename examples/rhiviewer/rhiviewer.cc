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
        urlLabel = new QLabel("-", q_ptr);
        urlLabel->setWordWrap(true);
        sizeLabel = new QLabel("-", q_ptr);
        fileSizeLabel = new QLabel("-", q_ptr);
        scaleLabel = new QLabel("-", q_ptr);
        imageListView = new ImageListView(q_ptr);
        imageListView->setFixedHeight(120);
    }

    RhiViewer *q_ptr;

    GpuGraphics::RhiView *rhiView;

    QLabel *backendNameLabel;
    QLabel *urlLabel;
    QLabel *fileSizeLabel;
    QLabel *sizeLabel;
    QLabel *scaleLabel;
    ImageInfoList imageInfoList;
    ImageListView *imageListView;

    QAtomicInteger<qint64> taskCount = 0;
};

RhiViewer::RhiViewer(QWidget *parent)
    : Viewer{parent}
    , d_ptr(new RhiViewerPrivate(this))
{
    setupUI();
    buildConnect();
}

RhiViewer::~RhiViewer()
{
    clearThumbnail();
}

bool RhiViewer::setImage(const QFileInfo &info, const QImage &image, const qint64 taskCount)
{
    if (taskCount != d_ptr->taskCount.loadRelaxed()) {
        return false;
    }
    QMetaObject::invokeMethod(this, [=] { onImageLoaded(info, image); }, Qt::QueuedConnection);
    return true;
}

void RhiViewer::onOpenImage()
{
    QString imageFilters(tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.svg *.tiff *.webp *.icns "
                            "*.bitmap *.graymap *.pixmap *.tga *.xbitmap *.xpixmap)"));
    //qDebug() << imageFilters;
    const QString path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                             .value(0, QDir::homePath());
    const QString filename = QFileDialog::getOpenFileName(this,
                                                          tr("Open Image"),
                                                          path,
                                                          imageFilters);
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

void RhiViewer::onScaleFactorChanged(qreal factor)
{
    const QString info = QString::number(factor * 100, 'f', 2) + QLatin1Char('%');
    d_ptr->scaleLabel->setText(info);
}

void RhiViewer::onImageSizeChanged(const QSize &size)
{
    QString imageSizeText;
    if (size.isValid()) {
        imageSizeText = QString::fromLatin1("%1x%2").arg(size.width()).arg(size.height());
    }
    d_ptr->sizeLabel->setText(imageSizeText);
}

void RhiViewer::onImageChanged(const QString &url)
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

void RhiViewer::onChangedImage(int index)
{
    if (index < 0 || index >= d_ptr->imageInfoList.size()) {
        return;
    }
    const ImageInfo &info = d_ptr->imageInfoList.at(index);
    d_ptr->rhiView->setImageUrl(info.fileInfo().absoluteFilePath());
}

void RhiViewer::onImageLoaded(const QFileInfo &fileInfo, const QImage &image)
{
    if (image.isNull()) {
        return;
    }
    d_ptr->imageInfoList.append(ImageInfo(fileInfo, image));
    d_ptr->imageListView->setImageInfoList(d_ptr->imageInfoList);
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
    layout->addWidget(d_ptr->imageListView);
}

QWidget *RhiViewer::toolWidget()
{
    auto *openImageButton = new QPushButton(tr("Open Picture"), this);
    connect(openImageButton, &QPushButton::clicked, this, &RhiViewer::onOpenImage);

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
    rightLayout->setSpacing(15);
    rightLayout->addWidget(openImageButton);
    rightLayout->addWidget(d_ptr->backendNameLabel);
    rightLayout->addWidget(infoBox);
    rightLayout->addStretch();

    return widget;
}

void RhiViewer::buildConnect()
{
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
    connect(d_ptr->imageListView, &ImageListView::changeItem, this, &RhiViewer::onChangedImage);
}

void RhiViewer::startImageLoadThread(const QString &url)
{
    d_ptr->taskCount.ref();
    QThreadPool::globalInstance()->start(
        new ImageLoadRunnable(url, this, d_ptr->taskCount.loadAcquire()));
}

void RhiViewer::clearThumbnail()
{
    if (d_ptr->imageInfoList.isEmpty()) {
        return;
    }
    d_ptr->imageInfoList.clear();
    d_ptr->imageListView->setImageInfoList(d_ptr->imageInfoList);
}

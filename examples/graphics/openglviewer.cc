#include "openglviewer.hpp"

#include <examples/common/imagelistmodel.h>
#include <gpugraphics/openglview.hpp>
#include <utils/utils.h>

#include <QtWidgets>

class OpenglViewer::OpenglViewerPrivate
{
public:
    OpenglViewerPrivate(OpenglViewer *q)
        : q_ptr(q)
    {
        openglView = new GpuGraphics::OpenglView(q_ptr);

        urlLabel = new QLabel("-", q_ptr);
        urlLabel->setWordWrap(true);
        sizeLabel = new QLabel("-", q_ptr);
        fileSizeLabel = new QLabel("-", q_ptr);
        scaleLabel = new QLabel("-", q_ptr);
        imageListView = new ImageListView(q_ptr);
        imageListView->setFixedHeight(120);
    }
    ~OpenglViewerPrivate() {}

    OpenglViewer *q_ptr;

    GpuGraphics::OpenglView *openglView;

    QLabel *urlLabel;
    QLabel *fileSizeLabel;
    QLabel *sizeLabel;
    QLabel *scaleLabel;
    ImageInfoList imageInfoList;
    ImageListView *imageListView;

    QAtomicInteger<qint64> taskCount = 0;
};

OpenglViewer::OpenglViewer(QWidget *parent)
    : Viewer(parent)
    , d_ptr(new OpenglViewerPrivate(this))
{
    setupUI();
    buildConnect();
}

OpenglViewer::~OpenglViewer()
{
    clearThumbnail();
}

bool OpenglViewer::setImage(const QFileInfo &info, const QImage &image, const qint64 taskCount)
{
    if (taskCount != d_ptr->taskCount.loadRelaxed()) {
        return false;
    }
    QMetaObject::invokeMethod(this, [=] { onImageLoaded(info, image); }, Qt::QueuedConnection);
    return true;
}

void OpenglViewer::onOpenImage()
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

    d_ptr->openglView->setImageUrl(filename);
}

void OpenglViewer::onScaleFactorChanged(qreal factor)
{
    const QString info = QString::number(factor * 100, 'f', 2) + QLatin1Char('%');
    d_ptr->scaleLabel->setText(info);
}

void OpenglViewer::onImageSizeChanged(const QSize &size)
{
    QString imageSizeText;
    if (size.isValid()) {
        imageSizeText = QString::fromLatin1("%1x%2").arg(size.width()).arg(size.height());
    }
    d_ptr->sizeLabel->setText(imageSizeText);
}

void OpenglViewer::onImageChanged(const QString &url)
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

void OpenglViewer::onChangedImage(int index)
{
    if (index < 0 || index >= d_ptr->imageInfoList.size()) {
        return;
    }
    const ImageInfo &info = d_ptr->imageInfoList.at(index);
    d_ptr->openglView->setImageUrl(info.fileInfo().absoluteFilePath());
}

void OpenglViewer::onImageLoaded(const QFileInfo &fileInfo, const QImage &image)
{
    if (image.isNull()) {
        return;
    }
    d_ptr->imageInfoList.append(ImageInfo(fileInfo, image));
    d_ptr->imageListView->setImageInfoList(d_ptr->imageInfoList);
}

void OpenglViewer::startImageLoadThread(const QString &url)
{
    d_ptr->taskCount.ref();
    QThreadPool::globalInstance()->start(
        new ImageLoadRunnable(url, this, d_ptr->taskCount.loadAcquire()));
}

void OpenglViewer::clearThumbnail()
{
    if (d_ptr->imageInfoList.isEmpty()) {
        return;
    }
    d_ptr->imageInfoList.clear();
    d_ptr->imageListView->setImageInfoList(d_ptr->imageInfoList);
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
    layout->addWidget(d_ptr->imageListView);
}

QWidget *OpenglViewer::toolWidget()
{
    auto *openImageButton = new QPushButton(tr("Open Picture"), this);
    connect(openImageButton, &QPushButton::clicked, this, &OpenglViewer::onOpenImage);

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

void OpenglViewer::buildConnect()
{
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
    connect(d_ptr->imageListView, &ImageListView::changeItem, this, &OpenglViewer::onChangedImage);
}
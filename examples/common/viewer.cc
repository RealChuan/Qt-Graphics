#include "viewer.hpp"
#include "thumbnailcache.hpp"

#include <utils/utils.hpp>

#include <QDir>
#include <QPointer>
#include <QThreadPool>
#include <QtWidgets>

Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

Viewer::~Viewer()
{
    ImageLoadRunnable::terminateAll();
}

bool Viewer::setThumbnail(const Thumbnail &thumbnail, const qint64 taskCount)
{
    if (taskCount != (m_taskCount.load() - 1)) {
        return false;
    }
    QMetaObject::invokeMethod(
        this, [this, thumbnail] { appendThumbnail(thumbnail); }, Qt::QueuedConnection);
    return true;
}

void Viewer::onScaleFactorChanged(qreal factor)
{
    const auto text = QString::number(factor * 100, 'f', 2) + QLatin1Char('%');
    m_scaleLabel->setText(text);
    m_scaleLabel->setToolTip(text);
}

void Viewer::onImageSizeChanged(const QSize &size)
{
    QString text;
    if (size.isValid()) {
        text = QString::fromLatin1("%1x%2").arg(size.width()).arg(size.height());
    }
    m_sizeLabel->setText(text);
    m_sizeLabel->setToolTip(text);
}

void Viewer::onImageChanged(const QString &url)
{
    m_urlLabel->setText(url);
    m_urlLabel->setToolTip(url);
    m_fileSizeLabel->setText(Utils::formatBytes(QFile(url).size()));

    for (const auto &data : std::as_const(m_thumbnailList)) {
        if (data.fileInfo().absoluteFilePath() == url) {
            return;
        }
    }
    clearThumbnail();
    startImageLoadThread(url);
}

QString Viewer::openImage()
{
    const QString imageFilters(
        tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.svg *.tiff *.webp *.icns "
           "*.bitmap *.graymap *.pixmap *.tga *.xbitmap *.xpixmap)"));
    const auto path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                          .value(0, QDir::homePath());
    return QFileDialog::getOpenFileName(this, tr("Open Image"), path, imageFilters);
}

void Viewer::startImageLoadThread(const QString &url)
{
    QThreadPool::globalInstance()->start(new ImageLoadRunnable(url, this, m_taskCount.fetch_add(1)));
}

void Viewer::appendThumbnail(const Thumbnail &thumbnail)
{
    m_thumbnailList.append(thumbnail);
    m_imageListView->setDatas(m_thumbnailList);
}

void Viewer::clearThumbnail()
{
    if (m_thumbnailList.isEmpty()) {
        return;
    }
    m_thumbnailList.clear();
    m_imageListView->setDatas(m_thumbnailList);
}

void Viewer::setupUI()
{
    m_openButton = new QToolButton(this);
    auto sizePolicy = m_openButton->sizePolicy();
    sizePolicy.setHorizontalPolicy(QSizePolicy::Preferred);
    m_openButton->setSizePolicy(sizePolicy);
    m_openButton->setText(tr("Open Picture"));

    m_infoBox = new QGroupBox(tr("Image Information"), this);
    m_urlLabel = new QLabel("-", this);
    m_urlLabel->setWordWrap(true);
    sizePolicy = m_urlLabel->sizePolicy();
    sizePolicy.setVerticalPolicy(QSizePolicy::MinimumExpanding);
    m_urlLabel->setSizePolicy(sizePolicy);
    m_sizeLabel = new QLabel("-", this);
    m_fileSizeLabel = new QLabel("-", this);
    m_scaleLabel = new QLabel("-", this);
    m_imageListView = new ImageListView(this);
    m_imageListView->setFixedHeight(120);

    auto *gridLayout = new QGridLayout(m_infoBox);
    gridLayout->addWidget(new QLabel(tr("Url: "), this), 0, 0, 1, 1);
    gridLayout->addWidget(m_urlLabel, 0, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("File Size: "), this), 1, 0, 1, 1);
    gridLayout->addWidget(m_fileSizeLabel, 1, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Image Size: "), this), 2, 0, 1, 1);
    gridLayout->addWidget(m_sizeLabel, 2, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Scaling Ratio:"), this), 3, 0, 1, 1);
    gridLayout->addWidget(m_scaleLabel, 3, 1, 1, 1);
}

class ImageLoadRunnable::ImageLoadRunnablePrivate
{
public:
    explicit ImageLoadRunnablePrivate(ImageLoadRunnable *q)
        : q_ptr(q)
    {}

    ImageLoadRunnable *q_ptr;

    QPointer<Viewer> viewPtr;
    QString fileUrl;
    std::atomic_llong taskCount;

    static std::atomic_bool running;
};

std::atomic_bool ImageLoadRunnable::ImageLoadRunnablePrivate::running = true;

ImageLoadRunnable::ImageLoadRunnable(const QString &fileUrl, Viewer *view, qint64 taskCount)
    : d_ptr(new ImageLoadRunnablePrivate(this))
{
    d_ptr->viewPtr = view;
    d_ptr->fileUrl = fileUrl;
    d_ptr->taskCount.store(taskCount);

    setAutoDelete(true);
}

ImageLoadRunnable::~ImageLoadRunnable() = default;

void ImageLoadRunnable::terminateAll()
{
    ImageLoadRunnablePrivate::running.store(false, std::memory_order_relaxed);
    auto *instance = QThreadPool::globalInstance();
    instance->clear();
    instance->waitForDone();
}

void ImageLoadRunnable::run()
{
    const QFileInfo file(d_ptr->fileUrl);
    if (!file.exists()) {
        return;
    }
    auto *thumbnailCacheInstance = ThumbnailCache::instance();
    QDirIterator it(file.absolutePath(), QDir::Files | QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        const QFileInfo info(it.next());
        Thumbnail thumbnail;
        thumbnail.setFileInfo(info);
        auto find = thumbnailCacheInstance->find(thumbnail);
        if (!find) {
            QImage image(info.absoluteFilePath());
            if (image.isNull()) {
                continue;
            }
            thumbnail.setImage(image);
        }
        auto image = thumbnail.image();
        if (image.width() > WIDTH || image.height() > WIDTH) {
            image = image.scaled(WIDTH, WIDTH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            thumbnail.setImage(image);
        }
        if (!find) {
            thumbnailCacheInstance->insert(thumbnail);
        }
        if (d_ptr->viewPtr.isNull()
            || !ImageLoadRunnablePrivate::running.load(std::memory_order_acquire)
            || !d_ptr->viewPtr->setThumbnail(thumbnail, d_ptr->taskCount.load())) {
            return;
        }
    }
}

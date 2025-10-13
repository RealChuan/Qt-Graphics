#include "imagecache.hpp"
#include "threadutils.h"
#include "utils.hpp"

#include <QCache>
#include <QImage>
#include <QtConcurrent>

namespace Utils {

static QImage syncLoadImage(const QString &absoluteFilePath)
{
    QEventLoop loop;
    QFutureWatcher<QImage> watcher;
    QObject::connect(&watcher, &QFutureWatcher<QImage>::finished, &loop, &QEventLoop::quit);
    watcher.setFuture(QtConcurrent::run([absoluteFilePath]() {
        return absoluteFilePath.isEmpty() ? QImage{} : QImage{absoluteFilePath};
    }));
    loop.exec();
    return watcher.result();
}

QString getCacheKey(const QFileInfo &fileInfo)
{
    if (!fileInfo.exists()) {
        return {};
    }
    auto temp = QString("%1|%2|%3")
                    .arg(fileInfo.absoluteFilePath(),
                         QString::number(fileInfo.lastModified().toSecsSinceEpoch()),
                         QString::number(fileInfo.size()));
    return QCryptographicHash::hash(temp.toLocal8Bit(), QCryptographicHash::Md5).toHex();
}

class ImageCache::ImageCachePrivate
{
public:
    explicit ImageCachePrivate(ImageCache *q)
        : q_ptr(q)
    {
        imageCache.setMaxCost(1024 * 1024);
    }

    void insert(const QString &key, const QImage &image)
    {
        Q_ASSERT(Utils::isMainThread());
        imageCache.insert(key, new QImage(image), qMax(qCeil(image.sizeInBytes() / 1024.0), 1));
    }

    bool find(const QString &key, QImage &image)
    {
        Q_ASSERT(Utils::isMainThread());
        auto *imagePtr = imageCache.object(key);
        if (!imagePtr) {
            return false;
        }
        image = *imagePtr;
        return true;
    }

    ImageCache *q_ptr;

    QCache<QString, QImage> imageCache;
};

void ImageCache::insert(const QString &absoluteFilePath, const QImage &image)
{
    d_ptr->insert(getCacheKey(QFileInfo(absoluteFilePath)), image);
}

bool ImageCache::find(const QString &absoluteFilePath, QImage &image)
{
    auto key = getCacheKey(QFileInfo(absoluteFilePath));
    auto ret = d_ptr->find(key, image);
    if (!ret) {
        image = syncLoadImage(absoluteFilePath);
        if (image.isNull()) {
            return false;
        }
        d_ptr->insert(key, image);
    }
    return true;
}

ImageCache::ImageCache(QObject *parent)
    : QObject{parent}
    , d_ptr{new ImageCachePrivate{this}}
{}

ImageCache::~ImageCache() {}

} // namespace Utils

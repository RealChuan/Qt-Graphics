#include "thumbnailcache.hpp"
#include "thumbnail.hpp"

#include <utils/utils.h>

#include <QCache>
#include <QMutex>

class ThumbnailCache::ThumbnailCachePrivate
{
public:
    explicit ThumbnailCachePrivate(ThumbnailCache *q)
        : q_ptr(q)
    {
        thumbnailCache.setMaxCost(100 * 1024);
        Utils::createDirectoryRecursively(cachePath);
        QThreadPool::globalInstance()->start(std::bind(&ThumbnailCachePrivate::cleanCache, this));
    }

    void insert(const QString &key, const QImage &image)
    {
        QMutexLocker locker(&mutex);
        thumbnailCache.insert(key, new QImage(image), qMax(qCeil(image.sizeInBytes() / 1024.0), 1));
    }

    bool find(const QString &key, QImage &image)
    {
        QMutexLocker locker(&mutex);
        auto *imagePtr = thumbnailCache.object(key);
        if (!imagePtr) {
            return false;
        }
        image = *imagePtr;
        return true;
    }

    void saveToDisk(const QString &key, const QImage &image)
    {
        const auto filePath(thumbnailCachePath(key));
        if (QFile::exists(filePath)) {
            return;
        }
        image.save(filePath, "png");
    }

    bool loadFromDisk(const QString &key, QImage &image)
    {
        const auto filePath(thumbnailCachePath(key));
        if (!QFile::exists(filePath)) {
            return false;
        }
        image.load(filePath);
        return !image.isNull();
    }

    QString thumbnailCachePath(const QString &key) const
    {
        return QString("%1/%2")
            .arg(cachePath,
                 QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Md5).toHex());
    }

    void cleanCache()
    {
        auto fileInfoList = QDir(cachePath).entryInfoList(QDir::Files | QDir::NoDotAndDotDot,
                                                          QDir::Time);
        qint64 maxSize = 100 * 1024 * 1024;
        qint64 size = 0;
        for (const auto &fileInfo : std::as_const(fileInfoList)) {
            if (size > maxSize) {
                QFile::remove(fileInfo.absoluteFilePath());
            } else {
                size += fileInfo.size();
            }
        }
    }

    ThumbnailCache *q_ptr;

    QCache<QString, QImage> thumbnailCache;
    QMutex mutex;
    QString cachePath = Utils::cachePath() + QStringLiteral("/thumbnails");
};

ThumbnailCache::ThumbnailCache(QObject *parent)
    : QObject(parent)
    , d_ptr(new ThumbnailCachePrivate(this))
{}

ThumbnailCache::~ThumbnailCache() {}

void ThumbnailCache::insert(const Thumbnail &thumbnail)
{
    auto image = thumbnail.image();
    if (image.isNull()) {
        return;
    }
    auto key = thumbnail.fileInfo().absoluteFilePath();

    d_ptr->insert(key, image);
    d_ptr->saveToDisk(key, image);
}

bool ThumbnailCache::find(Thumbnail &thumbnail)
{
    auto key = thumbnail.fileInfo().absoluteFilePath();
    QImage image;
    if (d_ptr->find(key, image)) {
        thumbnail.setImage(image);
        return true;
    }
    if (d_ptr->loadFromDisk(key, image)) {
        thumbnail.setImage(image);
        d_ptr->insert(key, image);
        return true;
    }
    return false;
}

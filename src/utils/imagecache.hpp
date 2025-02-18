#pragma once

#include "singleton.hpp"
#include "utils_global.h"

#include <QObject>

class QFileInfo;

namespace Utils {

UTILS_EXPORT QString getCacheKey(const QFileInfo &fileInfo);

class UTILS_EXPORT ImageCache : public QObject
{
    Q_OBJECT
public:
    void insert(const QString &absoluteFilePath, const QImage &image);
    bool find(const QString &absoluteFilePath, QImage &image);

private:
    explicit ImageCache(QObject *parent = nullptr);
    ~ImageCache() override;

    class ImageCachePrivate;
    QScopedPointer<ImageCachePrivate> d_ptr;

    SINGLETON(ImageCache)
};

} // namespace Utils

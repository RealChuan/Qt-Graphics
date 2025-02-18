#pragma once

#include <utils/singleton.hpp>

#include <QObject>

class Thumbnail;
class ThumbnailCache : public QObject
{
    Q_OBJECT
public:
    void insert(const Thumbnail &thumbnail);
    bool find(Thumbnail &thumbnail);

private:
    explicit ThumbnailCache(QObject *parent = nullptr);
    ~ThumbnailCache() override;

    class ThumbnailCachePrivate;
    QScopedPointer<ThumbnailCachePrivate> d_ptr;

    SINGLETON(ThumbnailCache)
};

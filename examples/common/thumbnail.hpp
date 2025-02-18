#pragma once

#include <QFileInfo>
#include <QImage>

#define WIDTH 85

class ThumbnailData : public QSharedData
{
public:
    ThumbnailData() {}
    ThumbnailData(const ThumbnailData &other)
        : QSharedData(other)
        , fileInfo(other.fileInfo)
        , image(other.image)
    {}
    ~ThumbnailData() {}

    QFileInfo fileInfo;
    QImage image;
};

class Thumbnail
{
public:
    Thumbnail()
        : d_ptr(new ThumbnailData)
    {}

    Thumbnail(const QFileInfo &fileInfo, const QImage &image)
        : d_ptr(new ThumbnailData)
    {
        setFileInfo(fileInfo);
        setImage(image);
    }

    Thumbnail(const Thumbnail &other)
        : d_ptr(other.d_ptr)
    {}

    ~Thumbnail() {}

    void setFileInfo(const QFileInfo &fileInfo) { d_ptr->fileInfo = fileInfo; }
    [[nodiscard]] auto fileInfo() const -> QFileInfo { return d_ptr->fileInfo; }

    void setImage(const QImage &image) { d_ptr->image = image; }
    [[nodiscard]] auto image() const -> QImage { return d_ptr->image; }

private:
    QExplicitlySharedDataPointer<ThumbnailData> d_ptr;
};

using ThumbnailList = QList<Thumbnail>;

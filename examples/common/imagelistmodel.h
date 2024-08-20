#ifndef IMAGELISTMODEL_H
#define IMAGELISTMODEL_H

#include <QAbstractListModel>
#include <QFileInfo>
#include <QListView>
#include <QPixmap>

#define WIDTH 85

class ImageData : public QSharedData
{
public:
    ImageData() {}
    ImageData(const ImageData &other)
        : QSharedData(other)
        , fileInfo(other.fileInfo)
        , image(other.image)
    {}
    ~ImageData() {}

    QFileInfo fileInfo;
    QImage image;
};

class ImageInfo
{
public:
    ImageInfo()
        : d_ptr(new ImageData)
    {}
    ImageInfo(const QFileInfo &fileInfo, const QImage &image)
        : d_ptr(new ImageData)
    {
        setFileInfo(fileInfo);
        setImage(image);
    }
    ImageInfo(const ImageInfo &other)
        : d_ptr(other.d_ptr)
    {}

    void setFileInfo(const QFileInfo &fileInfo) { d_ptr->fileInfo = fileInfo; }
    [[nodiscard]] auto fileInfo() const -> QFileInfo { return d_ptr->fileInfo; }

    void setImage(const QImage &image) { d_ptr->image = image; }
    [[nodiscard]] auto image() const -> QImage { return d_ptr->image; }

private:
    QExplicitlySharedDataPointer<ImageData> d_ptr;
};

using ImageInfoList = QList<ImageInfo>;

class ImageListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ImageListModel(QObject *parent = nullptr);
    ~ImageListModel() override;
    [[nodiscard]] auto rowCount(const QModelIndex & = QModelIndex()) const -> int override;
    [[nodiscard]] auto data(const QModelIndex &index, int role = Qt::DisplayRole) const
        -> QVariant override;

    void setImageInfoList(const ImageInfoList &);

private:
    struct ImageListModelPrivate;
    QScopedPointer<ImageListModelPrivate> d_ptr;
};

class ImageListView : public QListView
{
    Q_OBJECT
public:
    explicit ImageListView(QWidget *parent = nullptr);
    ~ImageListView() override;

    void setImageInfoList(const ImageInfoList &);

signals:
    void changeItem(int);

private slots:
    void onChangedItem(const QModelIndex &index);

private:
    void setupUI();

    class ImageListViewPrivate;
    QScopedPointer<ImageListViewPrivate> d_ptr;
};

#endif // IMAGELISTMODEL_H

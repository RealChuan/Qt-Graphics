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
    QFileInfo fileInfo() const { return d_ptr->fileInfo; }

    void setImage(const QImage &image) { d_ptr->image = image; }
    QImage image() const { return d_ptr->image; }

private:
    QExplicitlySharedDataPointer<ImageData> d_ptr;
};

typedef QList<ImageInfo> ImageInfoList;

class ImageListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ImageListModel(QObject *parent = nullptr);
    ~ImageListModel();
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

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
    ~ImageListView();

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

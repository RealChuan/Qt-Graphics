#include "imagelistmodel.h"

#include <QScrollBar>

class ImageListModel::ImageListModelPrivate
{
public:
    explicit ImageListModelPrivate(ImageListModel *q)
        : q_ptr(q)
    {}

    ImageListModel *q_ptr;

    ThumbnailList datas;
};

ImageListModel::ImageListModel(QObject *parent)
    : QAbstractListModel(parent)
    , d_ptr(new ImageListModelPrivate(this))
{}

ImageListModel::~ImageListModel() {}

auto ImageListModel::rowCount(const QModelIndex &) const -> int
{
    return d_ptr->datas.size();
}

auto ImageListModel::data(const QModelIndex &index, int role) const -> QVariant
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto data = d_ptr->datas.at(index.row());
    switch (role) {
    case Qt::DecorationRole: return QPixmap::fromImage(data.image());
    case Qt::WhatsThisRole:
    case Qt::ToolTipRole: return data.fileInfo().fileName();
    case Qt::SizeHintRole: return QSize(90, 90);
    case Qt::TextAlignmentRole: return Qt::AlignCenter;
    default: break;
    }
    return QVariant();
}

void ImageListModel::setDatas(const ThumbnailList &datas)
{
    beginResetModel();
    d_ptr->datas = datas;
    endResetModel();
}

class ImageListView::ImageListViewPrivate
{
public:
    explicit ImageListViewPrivate(ImageListView *q)
        : q_ptr(q)
    {
        imageListModel = new ImageListModel(q_ptr);
    }

    ImageListView *q_ptr;
    ImageListModel *imageListModel;
};

ImageListView::ImageListView(QWidget *parent)
    : QListView(parent)
    , d_ptr(new ImageListViewPrivate(this))
{
    setupUI();
    connect(this, &QListView::doubleClicked, this, &ImageListView::onChangedItem);
}

ImageListView::~ImageListView() {}

void ImageListView::setDatas(const ThumbnailList &datas)
{
    d_ptr->imageListModel->setDatas(datas);
}

void ImageListView::onChangedItem(const QModelIndex &index)
{
    emit changeItem(index.row());
}

void ImageListView::setupUI()
{
    setModel(d_ptr->imageListModel);
    setFlow(LeftToRight);
    setResizeMode(Adjust);
    setIconSize(QSize(WIDTH, WIDTH));
}

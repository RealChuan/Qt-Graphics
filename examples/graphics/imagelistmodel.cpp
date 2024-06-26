#include "imagelistmodel.h"

#include <QScrollBar>

struct ImageListModel::ImageListModelPrivate
{
    ImageInfoList imageInfoList;
};

ImageListModel::ImageListModel(QObject *parent)
    : QAbstractListModel(parent)
    , d_ptr(new ImageListModelPrivate)
{}

ImageListModel::~ImageListModel() {}

auto ImageListModel::rowCount(const QModelIndex &) const -> int
{
    return d_ptr->imageInfoList.size();
}

auto ImageListModel::data(const QModelIndex &index, int role) const -> QVariant
{
    if (!index.isValid()) {
        return QVariant();
    }

    ImageInfo imageInfo = d_ptr->imageInfoList.at(index.row());
    switch (role) {
    case Qt::DecorationRole: return QIcon(QPixmap::fromImage(imageInfo.image()));
    case Qt::WhatsThisRole:
    case Qt::ToolTipRole: return imageInfo.fileInfo().fileName();
    case Qt::SizeHintRole: return QSize(90, 90);
    case Qt::TextAlignmentRole: return Qt::AlignCenter;
    default: break;
    }
    return QVariant();
}

void ImageListModel::setImageInfoList(const ImageInfoList &imageInfoList)
{
    beginResetModel();
    d_ptr->imageInfoList = imageInfoList;
    endResetModel();
}

/*
 *
 * */

class ImageListView::ImageListViewPrivate
{
public:
    ImageListViewPrivate(QWidget *parent)
        : q_ptr(parent)
    {
        imageListModel = new ImageListModel(q_ptr);
    }
    QWidget *q_ptr;
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

void ImageListView::setImageInfoList(const ImageInfoList &imageVector)
{
    d_ptr->imageListModel->setImageInfoList(imageVector);
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

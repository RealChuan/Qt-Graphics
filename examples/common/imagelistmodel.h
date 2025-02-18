#pragma once

#include "thumbnail.hpp"

#include <QAbstractListModel>
#include <QFileInfo>
#include <QListView>
#include <QPixmap>

class ImageListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ImageListModel(QObject *parent = nullptr);
    ~ImageListModel() override;
    [[nodiscard]] auto rowCount(const QModelIndex & = QModelIndex()) const -> int override;
    [[nodiscard]] auto data(const QModelIndex &index, int role = Qt::DisplayRole) const
        -> QVariant override;

    void setDatas(const ThumbnailList &datas);

private:
    class ImageListModelPrivate;
    QScopedPointer<ImageListModelPrivate> d_ptr;
};

class ImageListView : public QListView
{
    Q_OBJECT
public:
    explicit ImageListView(QWidget *parent = nullptr);
    ~ImageListView() override;

    void setDatas(const ThumbnailList &datas);

signals:
    void changeItem(int);

private slots:
    void onChangedItem(const QModelIndex &index);

private:
    void setupUI();

    class ImageListViewPrivate;
    QScopedPointer<ImageListViewPrivate> d_ptr;
};

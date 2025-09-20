#pragma once

#include <utils/icowriter.hpp>

#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include <QWidget>

class SizeSelectorModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SizeSelectorModel(QObject *parent = nullptr);
    ~SizeSelectorModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override { return 2; }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setSizes(const Utils::ImageSizes &sizes);
    Utils::ImageSizes sizes() const;

private:
    class SizeSelectorModelPrivate;
    QScopedPointer<SizeSelectorModelPrivate> d_ptr;
};

class SizeSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SizeSelectorWidget(QWidget *parent = nullptr);
    ~SizeSelectorWidget() override;

    Utils::ImageSizes sizes() const;
    void setSizes(const Utils::ImageSizes &sizes);

private:
    void setupUI();
    void buildConnect();

    class SizeSelectorWidgetPrivate;
    QScopedPointer<SizeSelectorWidgetPrivate> d_ptr;
};

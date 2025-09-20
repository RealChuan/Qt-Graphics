#include "sizeselectorwidget.hpp"

#include <QtWidgets>

class SizeSelectorModel::SizeSelectorModelPrivate
{
public:
    explicit SizeSelectorModelPrivate(SizeSelectorModel *q)
        : q_ptr(q)
    {}

    SizeSelectorModel *q_ptr;

    struct SizeData
    {
        int size = 0;
        bool checked = true;
    };
    using SizeDatas = QList<SizeData>;
    SizeDatas datas;
};

SizeSelectorModel::SizeSelectorModel(QObject *parent)
    : QAbstractTableModel(parent)
    , d_ptr(new SizeSelectorModelPrivate(this))
{}

SizeSelectorModel::~SizeSelectorModel() {}

int SizeSelectorModel::rowCount(const QModelIndex &parent) const
{
    return d_ptr->datas.size();
}

QVariant SizeSelectorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    auto row = index.row();
    auto col = index.column();

    const auto &data = d_ptr->datas.at(row);
    switch (role) {
    case Qt::TextAlignmentRole: return Qt::AlignCenter;
    case Qt::CheckStateRole:
        if (0 == col) {
            return data.checked ? Qt::Checked : Qt::Unchecked;
        }
        break;
    case Qt::WhatsThisRole:
    case Qt::ToolTipRole:
    case Qt::DisplayRole: {
        switch (col) {
        case 1: return data.size;
        default: break;
        }
        break;
    }
    default: break;
    }
    return {};
}

bool SizeSelectorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    auto row = index.row();
    auto col = index.column();
    auto &data = d_ptr->datas[row];
    switch (role) {
    case Qt::CheckStateRole:
        if (0 == col) {
            data.checked = value.toBool();
            emit dataChanged(index, index, {role});
            return true;
        }
        break;
    default: break;
    }
    return false;
}

Qt::ItemFlags SizeSelectorModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return {};
    }
    auto flags = QAbstractTableModel::flags(index);
    if (0 == index.column()) {
        flags |= Qt::ItemIsUserCheckable;
    }
    return flags;
}

void SizeSelectorModel::setSizes(const Utils::ImageSizes &sizes)
{
    auto copy = sizes;
    std::sort(copy.begin(), copy.end(), std::greater<int>());
    SizeSelectorModelPrivate::SizeDatas tmp;
    for (const auto &size : std::as_const(copy)) {
        tmp.append({size, true});
    }
    beginResetModel();
    d_ptr->datas = tmp;
    endResetModel();
}

Utils::ImageSizes SizeSelectorModel::sizes() const
{
    Utils::ImageSizes datas;
    for (const auto &data : std::as_const(d_ptr->datas)) {
        if (data.checked) {
            datas.append(data.size);
        }
    }
    return datas;
}

class SizeSelectorWidget::SizeSelectorWidgetPrivate
{
public:
    explicit SizeSelectorWidgetPrivate(SizeSelectorWidget *q)
        : q_ptr(q)
    {
        tableView = new QTableView(q_ptr);
        model = new SizeSelectorModel(q_ptr);
        tableView->setModel(model);
        tableView->setAlternatingRowColors(true);
        tableView->setIconSize(QSize(20, 20));

        auto *verticalHeader = tableView->verticalHeader();
        verticalHeader->setVisible(false);
        verticalHeader->setDefaultSectionSize(10);
        auto *horizontalHeader = tableView->horizontalHeader();
        horizontalHeader->setVisible(false);
        horizontalHeader->setStretchLastSection(true);
        horizontalHeader->setDefaultSectionSize(50);
        horizontalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    }

    SizeSelectorWidget *q_ptr;

    QTableView *tableView;
    SizeSelectorModel *model;
};

SizeSelectorWidget::SizeSelectorWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SizeSelectorWidgetPrivate(this))
{
    setupUI();
    buildConnect();
}

SizeSelectorWidget::~SizeSelectorWidget() {}

Utils::ImageSizes SizeSelectorWidget::sizes() const
{
    return d_ptr->model->sizes();
}

void SizeSelectorWidget::setSizes(const Utils::ImageSizes &sizes)
{
    d_ptr->model->setSizes(sizes);
}

void SizeSelectorWidget::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->addWidget(d_ptr->tableView);
}

void SizeSelectorWidget::buildConnect() {}

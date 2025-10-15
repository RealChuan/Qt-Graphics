#include "maskdialog.h"
#include "stretchparamssettingdailog.hpp"

#include <graphics/graphicspixmapitem.h>
#include <graphics/graphicsview.hpp>

#include <QtWidgets>

using namespace Graphics;

class MaskDialog::MaskDialogPrivate
{
public:
    MaskDialogPrivate(QWidget *parent)
        : q_ptr(parent)
    {
        imageView = new GraphicsView(q_ptr);
        buttonGroup = new QButtonGroup(q_ptr);
        buttonGroup->setExclusive(true);
        penSizeSpinBox = new QSpinBox(q_ptr);
        penSizeSpinBox->setKeyboardTracking(false);
        opacitySpinBox = new QDoubleSpinBox(q_ptr);
        opacitySpinBox->setKeyboardTracking(false);
        opacitySpinBox->setRange(0, 1);
        opacitySpinBox->setSingleStep(0.1);
        opacitySpinBox->setDecimals(1);
    }
    QWidget *q_ptr;

    GraphicsView *imageView;
    QButtonGroup *buttonGroup;
    QSpinBox *penSizeSpinBox;
    QDoubleSpinBox *opacitySpinBox;

    QString name;
};

MaskDialog::MaskDialog(QWidget *parent)
    : QDialog(parent)
    , d_ptr(new MaskDialogPrivate(this))
{
    setWindowTitle(tr("Mask Edit"));
    setupUI();
    buildConnect();
    resize(parent->size());
    show();
}

MaskDialog::~MaskDialog() {}

void MaskDialog::setImageName(const QString &name)
{
    d_ptr->name = name;
    setWindowTitle(tr("Mask Edit-%1").arg(name));
}

void MaskDialog::setPixmap(const QPixmap &pixmap)
{
    d_ptr->imageView->setPixmap(pixmap);

    d_ptr->opacitySpinBox->setValue(0.5);
    int min = qMin(pixmap.width(), pixmap.height());
    d_ptr->penSizeSpinBox->setRange(1, min);
    d_ptr->penSizeSpinBox->setValue(min / 20.0);
}

QImage MaskDialog::maskImage()
{
    GraphicsPixmapItem *pixmapItem = d_ptr->imageView->pixmapItem();
    if (!pixmapItem) {
        return QImage();
    }
    return pixmapItem->maskImage();
}

void MaskDialog::setMaskImage(const QImage &image)
{
    GraphicsPixmapItem *pixmapItem = d_ptr->imageView->pixmapItem();
    if (!pixmapItem) {
        return;
    }
    pixmapItem->setMaskImage(image);
}

void MaskDialog::onButtonClicked(int id)
{
    GraphicsPixmapItem *pixmapItem = d_ptr->imageView->pixmapItem();
    if (!pixmapItem) {
        return;
    }
    GraphicsPixmapItem::Mode mode = GraphicsPixmapItem::Normal;
    switch (id) {
    case 1: mode = GraphicsPixmapItem::MaskDraw; break;
    case 2: mode = GraphicsPixmapItem::MaskErase; break;
    case 3: pixmapItem->clearMask(); break;
    default: break;
    }
    pixmapItem->setPaintMode(mode);
}

void MaskDialog::onPenSizeChanged(int value)
{
    GraphicsPixmapItem *pixmapItem = d_ptr->imageView->pixmapItem();
    if (!pixmapItem) {
        return;
    }
    pixmapItem->setPenSize(value);
}

void MaskDialog::onOpacityChanged(double value)
{
    GraphicsPixmapItem *pixmapItem = d_ptr->imageView->pixmapItem();
    if (!pixmapItem) {
        return;
    }
    pixmapItem->setOpacity(value);
}

void MaskDialog::onSave()
{
    GraphicsPixmapItem *pixmapItem = d_ptr->imageView->pixmapItem();
    if (!pixmapItem) {
        return;
    }

    QPixmap pixmap = pixmapItem->pixmap();
    if (pixmap.isNull()) {
        return;
    }

    StretchParamsSettingDailog::StretchParams params{pixmap.size(), Qt::KeepAspectRatio};
    StretchParamsSettingDailog dialog(this);
    dialog.setParams(params);
    if (dialog.exec() != StretchParamsSettingDailog::Accepted) {
        return;
    }
    params = dialog.params();

    QString path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                       .value(0, QDir::homePath());
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    time = "~" + time;
    QString name = d_ptr->name;
    int index = name.lastIndexOf('.');
    if (index <= 0) {
        name = name + time + ".png";
    } else {
        name.insert(index, time);
    }
    path = path + "/" + name;
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Image"),
                                                    path,
                                                    tr("Images (*.png *.xpm *.jpg)"));
    if (filename.isEmpty()) {
        return;
    }
    QImage mask = pixmapItem->maskImage();
    QPainter painter(&pixmap);
    painter.setOpacity(d_ptr->opacitySpinBox->value());
    painter.drawImage(pixmap.rect(), mask);
    painter.end();
    pixmap = pixmap.scaled(params.size, params.mode, Qt::SmoothTransformation);
    qDebug() << pixmap.save(filename, nullptr, params.quality);
}

auto newFlatBlueButton(const QString &text, QWidget *parent) -> QPushButton *
{
    auto *button = new QPushButton(text, parent);
    return button;
}

void MaskDialog::setupUI()
{
    auto *button1 = newFlatBlueButton(tr("Mask"), this);
    auto *button2 = newFlatBlueButton(tr("Erase"), this);
    auto *button3 = new QPushButton(tr("Clear"), this);
    d_ptr->buttonGroup->addButton(button1, 1);
    d_ptr->buttonGroup->addButton(button2, 2);
    d_ptr->buttonGroup->addButton(button3, 3);
    d_ptr->buttonGroup->button(1)->setChecked(true);

    auto *saveButton = newFlatBlueButton(tr("Save"), this);
    connect(saveButton, &QPushButton::clicked, this, &MaskDialog::onSave);

    auto *editLayout = new QHBoxLayout;
    editLayout->addStretch();
    editLayout->addWidget(new QLabel(tr("Pen Size: "), this));
    editLayout->addWidget(d_ptr->penSizeSpinBox);
    editLayout->addWidget(new QLabel(tr("Mask Opacity: "), this));
    editLayout->addWidget(d_ptr->opacitySpinBox);
    editLayout->addWidget(button1);
    editLayout->addWidget(button2);
    editLayout->addWidget(button3);
    editLayout->addWidget(saveButton);
    editLayout->addStretch();

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 10);
    layout->addWidget(d_ptr->imageView);
    layout->addLayout(editLayout);
}

void MaskDialog::buildConnect()
{
    connect(d_ptr->buttonGroup, &QButtonGroup::idClicked, this, &MaskDialog::onButtonClicked);
    connect(d_ptr->penSizeSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &MaskDialog::onPenSizeChanged);
    connect(d_ptr->opacitySpinBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &MaskDialog::onOpacityChanged);
}

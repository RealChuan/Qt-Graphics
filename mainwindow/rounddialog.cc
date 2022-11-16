#include "rounddialog.hpp"
#include "stretchparamssettingdailog.hpp"

#include <graphics/graphicspixmapitem.h>
#include <graphics/graphicsrectitem.h>
#include <graphics/imageview.h>

#include <QtWidgets>

using namespace Graphics;

class RoundDialog::RoundDialogPrivate
{
public:
    RoundDialogPrivate(QWidget *parent)
        : owner(parent)
        , graphicsRectItemPtr(new GraphicsRectItem)
    {
        imageView = new ImageView(owner);
        buttonGroup = new QButtonGroup(owner);
        buttonGroup->setExclusive(true);

        cropWidget = new QWidget(owner);
        rectGroupBox = new QGroupBox(QObject::tr("Rect Info"), owner);
        topLeftXSpinBox = new QSpinBox(owner);
        topLeftXSpinBox->setKeyboardTracking(false);
        topLeftYSpinBox = new QSpinBox(owner);
        topLeftYSpinBox->setKeyboardTracking(false);
        widthSpinBox = new QSpinBox(owner);
        widthSpinBox->setKeyboardTracking(false);
        heightSpinBox = new QSpinBox(owner);
        heightSpinBox->setKeyboardTracking(false);

        radiusSpinBox = new QSpinBox(owner);
        radiusSpinBox->setKeyboardTracking(false);
    }
    ~RoundDialogPrivate() {}

    QWidget *owner;

    ImageView *imageView;

    QWidget *cropWidget;
    QButtonGroup *buttonGroup;
    QGroupBox *rectGroupBox;
    QSpinBox *topLeftXSpinBox;
    QSpinBox *topLeftYSpinBox;
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;

    QSpinBox *radiusSpinBox;

    QString name;

    QScopedPointer<GraphicsRectItem> graphicsRectItemPtr;
    QPixmap pixmap;
    QPixmap cutPixmap;
};

RoundDialog::RoundDialog(QWidget *parent)
    : QDialog(parent)
    , d_ptr(new RoundDialogPrivate(this))
{
    setWindowTitle(tr("Round Edit"));
    setupUI();
    buildConnect();
    resize(parent->size());
    show();
}

RoundDialog::~RoundDialog() {}

void RoundDialog::setImageName(const QString &name)
{
    d_ptr->name = name;
    setWindowTitle(tr("Round Edit-%1").arg(name));
}

void RoundDialog::setPixmap(const QPixmap &pixmap)
{
    d_ptr->pixmap = pixmap;
    d_ptr->topLeftXSpinBox->setRange(0, pixmap.width());
    d_ptr->topLeftYSpinBox->setRange(0, pixmap.height());
    d_ptr->widthSpinBox->setRange(0, pixmap.width());
    d_ptr->heightSpinBox->setRange(0, pixmap.height());
    d_ptr->imageView->setPixmap(pixmap);
    d_ptr->graphicsRectItemPtr->setRect(pixmap.rect().adjusted(10, 10, -10, -10));
    if (!d_ptr->imageView->scene()->items().contains(d_ptr->graphicsRectItemPtr.data())) {
        d_ptr->imageView->scene()->addItem(d_ptr->graphicsRectItemPtr.data());
    }
    buildConnect2();
}

void RoundDialog::onStartRound(bool checked)
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button) {
        return;
    }
    if (checked) {
        if (d_ptr->buttonGroup->button(1)->isChecked()) {
            d_ptr->cutPixmap = d_ptr->pixmap;
        } else {
            d_ptr->cutPixmap = d_ptr->pixmap.copy(d_ptr->graphicsRectItemPtr->rect().toRect());
        }
        d_ptr->imageView->setPixmap(d_ptr->cutPixmap);
        d_ptr->graphicsRectItemPtr->setRect(d_ptr->cutPixmap.rect().adjusted(10, 10, -10, -10));
        d_ptr->cropWidget->hide();
        d_ptr->graphicsRectItemPtr->hide();
        button->setText(tr("Reset Crop"));
        d_ptr->radiusSpinBox->setRange(0,
                                       qMin(d_ptr->cutPixmap.size().width(),
                                            d_ptr->cutPixmap.size().height())
                                           / 2.0);
        return;
    }
    d_ptr->cropWidget->show();
    d_ptr->rectGroupBox->show();
    setPixmap(d_ptr->pixmap);
    d_ptr->graphicsRectItemPtr->show();
    button->setText(tr("Start Round"));
}

void RoundDialog::onSave()
{
    if (d_ptr->cropWidget->isVisible()) {
        return;
    }

    QPixmap pixmap(d_ptr->imageView->pixmap());
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
    const QString time = "~" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    QString name = d_ptr->name;
    int index = name.lastIndexOf('.');
    if (index > 0) {
        name.resize(index);
    }
    name = name + time + ".png"; // PNG 才有alpha通道（透明度）
    path = path + "/" + name;
    const QString filename = QFileDialog::getSaveFileName(this,
                                                          tr("Save Image"),
                                                          path,
                                                          tr("Images (*.png *.xpm *.jpg)"));
    if (filename.isEmpty()) {
        return;
    }
    pixmap = pixmap.scaled(params.size, params.mode, Qt::SmoothTransformation);
    qDebug() << pixmap.save(filename, "PNG", params.quality);
}

void RoundDialog::onButtonClicked(int id)
{
    GraphicsPixmapItem *pixmapItem = d_ptr->imageView->pixmapItem();
    if (!pixmapItem) {
        return;
    }
    switch (id) {
    case 1:
        d_ptr->graphicsRectItemPtr->hide();
        d_ptr->rectGroupBox->hide();
        d_ptr->radiusSpinBox
            ->setRange(0, qMin(d_ptr->widthSpinBox->value(), d_ptr->heightSpinBox->value()) / 2);
        break;
    case 2:
        d_ptr->graphicsRectItemPtr->show();
        d_ptr->rectGroupBox->show();
        break;
    default: break;
    }
}

void RoundDialog::onRectChanged(const QRectF &rectF)
{
    d_ptr->topLeftXSpinBox->blockSignals(true);
    d_ptr->topLeftYSpinBox->blockSignals(true);
    d_ptr->widthSpinBox->blockSignals(true);
    d_ptr->heightSpinBox->blockSignals(true);
    d_ptr->topLeftXSpinBox->setValue(rectF.x());
    d_ptr->topLeftYSpinBox->setValue(rectF.y());
    d_ptr->widthSpinBox->setValue(rectF.width());
    d_ptr->heightSpinBox->setValue(rectF.height());
    d_ptr->topLeftXSpinBox->blockSignals(false);
    d_ptr->topLeftYSpinBox->blockSignals(false);
    d_ptr->widthSpinBox->blockSignals(false);
    d_ptr->heightSpinBox->blockSignals(false);

    d_ptr->widthSpinBox->setRange(0, d_ptr->pixmap.width() - rectF.x());
    d_ptr->heightSpinBox->setRange(0, d_ptr->pixmap.height() - rectF.y());
}

void RoundDialog::onTopLeftXChanged(int value)
{
    QRectF rectF = d_ptr->graphicsRectItemPtr->rect();
    QPointF point1 = rectF.topLeft();
    point1.setX(value);
    d_ptr->graphicsRectItemPtr->setRect(QRectF(point1, rectF.bottomRight()));
    d_ptr->graphicsRectItemPtr->update();
}

void RoundDialog::onTopLeftYChanged(int value)
{
    QRectF rectF = d_ptr->graphicsRectItemPtr->rect();
    QPointF point1 = rectF.topLeft();
    point1.setY(value);
    d_ptr->graphicsRectItemPtr->setRect(QRectF(point1, rectF.bottomRight()));
    d_ptr->graphicsRectItemPtr->update();
}

void RoundDialog::onWidthChanged(int value)
{
    QRectF rectF = d_ptr->graphicsRectItemPtr->rect();
    QPointF point1 = rectF.bottomRight();
    point1.setX(value + rectF.x());
    d_ptr->graphicsRectItemPtr->setRect(QRectF(rectF.topLeft(), point1));
    d_ptr->graphicsRectItemPtr->update();
}

void RoundDialog::onHeightChanged(int value)
{
    QRectF rectF = d_ptr->graphicsRectItemPtr->rect();
    QPointF point1 = rectF.bottomRight();
    point1.setY(value + rectF.y());
    d_ptr->graphicsRectItemPtr->setRect(QRectF(rectF.topLeft(), point1));
    d_ptr->graphicsRectItemPtr->update();
}

QImage radiusImage(const QPixmap &pixmap, int radius)
{
    QImage image(pixmap.size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setBrush(pixmap);
    painter.setPen(Qt::transparent);
    painter.drawRoundedRect(pixmap.rect(), radius, radius);
    return image;
}

void RoundDialog::onRadiusChanged(int value)
{
    if (d_ptr->cropWidget->isVisible()) {
        return;
    }

    d_ptr->imageView->setPixmap(QPixmap::fromImage(radiusImage(d_ptr->cutPixmap, value)));
}

void RoundDialog::setupUI()
{
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(d_ptr->imageView);
    splitter->addWidget(toolWidget());
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({INT_MAX, 1});

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(QMargins());
    layout->addWidget(splitter);
}

void RoundDialog::buildConnect()
{
    connect(d_ptr->buttonGroup, &QButtonGroup::idClicked, this, &RoundDialog::onButtonClicked);
    connect(d_ptr->graphicsRectItemPtr.data(),
            &Graphics::GraphicsRectItem::rectChanged,
            this,
            &RoundDialog::onRectChanged);
    connect(d_ptr->radiusSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &RoundDialog::onRadiusChanged);
    d_ptr->buttonGroup->button(1)->click();
}

void RoundDialog::buildConnect2()
{
    connect(d_ptr->topLeftXSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &RoundDialog::onTopLeftXChanged,
            Qt::UniqueConnection);
    connect(d_ptr->topLeftYSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &RoundDialog::onTopLeftYChanged,
            Qt::UniqueConnection);
    connect(d_ptr->widthSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &RoundDialog::onWidthChanged,
            Qt::UniqueConnection);
    connect(d_ptr->heightSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &RoundDialog::onHeightChanged,
            Qt::UniqueConnection);
}

QWidget *RoundDialog::toolWidget()
{
    QRadioButton *radioButton1 = new QRadioButton(tr("Original Image"), this);
    QRadioButton *radioButton2 = new QRadioButton(tr("Rect Crop"), this);
    d_ptr->buttonGroup->addButton(radioButton1, 1);
    d_ptr->buttonGroup->addButton(radioButton2, 2);

    QFormLayout *formLayout = new QFormLayout(d_ptr->rectGroupBox);
    formLayout->addRow(tr("TopLeft X:"), d_ptr->topLeftXSpinBox);
    formLayout->addRow(tr("TopLeft Y:"), d_ptr->topLeftYSpinBox);
    formLayout->addRow(tr("Width:"), d_ptr->widthSpinBox);
    formLayout->addRow(tr("Height:"), d_ptr->heightSpinBox);

    QVBoxLayout *cropLayout = new QVBoxLayout(d_ptr->cropWidget);
    cropLayout->setContentsMargins(QMargins());
    cropLayout->addWidget(radioButton1);
    cropLayout->addWidget(radioButton2);
    cropLayout->addWidget(d_ptr->rectGroupBox);

    QPushButton *startRoundButton = new QPushButton(tr("Start Round"), this);
    startRoundButton->setCheckable(true);
    connect(startRoundButton, &QPushButton::clicked, this, &RoundDialog::onStartRound);

    QFormLayout *radiusLayout = new QFormLayout;
    radiusLayout->addRow(tr("Radius:"), d_ptr->radiusSpinBox);

    QPushButton *saveButton = new QPushButton(tr("Save"), this);
    connect(saveButton, &QPushButton::clicked, this, &RoundDialog::onSave);

    QWidget *toolWidget = new QWidget(this);
    toolWidget->setMaximumWidth(250);
    QVBoxLayout *toolLayout = new QVBoxLayout(toolWidget);
    toolLayout->addWidget(d_ptr->cropWidget);
    toolLayout->addWidget(startRoundButton);
    toolLayout->addLayout(radiusLayout);
    toolLayout->addWidget(saveButton);
    toolLayout->addStretch();
    return toolWidget;
}

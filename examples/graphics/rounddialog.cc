#include "rounddialog.hpp"
#include "stretchparamssettingdailog.hpp"

#include <graphics/graphicspixmapitem.h>
#include <graphics/graphicsroundedrectitem.hpp>
#include <graphics/graphicsview.hpp>

#include <QtWidgets>

using namespace Graphics;

auto radiusImage(const QPixmap &pixmap, int radius) -> QImage
{
    QImage image(pixmap.size(), QImage::Format_RGBA8888_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setBrush(pixmap);
    painter.setPen(Qt::transparent);
    painter.drawRoundedRect(pixmap.rect(), radius, radius);
    return image;
}

class RoundDialog::RoundDialogPrivate
{
public:
    explicit RoundDialogPrivate(RoundDialog *q)
        : q_ptr(q)
        , roundedRectItemPtr(new GraphicsRoundedRectItem)
    {
        roundedRectItemPtr->setShowBoundingRect(true);

        imageView = new GraphicsView(q_ptr);

        previewLabel = new QLabel(q_ptr);
        previewLabel->setMinimumHeight(200);
        previewLabel->setAlignment(Qt::AlignCenter);

        rectGroupBox = new QGroupBox(QObject::tr("Rounded rect info"), q_ptr);
        topLeftXSpinBox = new QSpinBox(q_ptr);
        topLeftXSpinBox->setKeyboardTracking(false);
        topLeftYSpinBox = new QSpinBox(q_ptr);
        topLeftYSpinBox->setKeyboardTracking(false);
        widthSpinBox = new QSpinBox(q_ptr);
        widthSpinBox->setKeyboardTracking(false);
        heightSpinBox = new QSpinBox(q_ptr);
        heightSpinBox->setKeyboardTracking(false);
        radiusSpinBox = new QSpinBox(q_ptr);
        radiusSpinBox->setKeyboardTracking(false);
    }
    ~RoundDialogPrivate() {}

    void setRoundedRect(const RoundedRect &roundedRect)
    {
        roundedRectItemPtr->setRoundedRect(roundedRect);
        radiusSpinBox->setRange(0, qMin(roundedRect.rect.width(), roundedRect.rect.height()) / 2.0);
        if (radiusSpinBox->value() > radiusSpinBox->maximum()) {
            radiusSpinBox->setValue(radiusSpinBox->maximum());
        }

        QMetaObject::invokeMethod(
            roundedRectItemPtr.data(), [=] { roundedRectItemPtr->update(); }, Qt::QueuedConnection);
        updatePreview();
    }

    void updatePreview()
    {
        static QAtomicInt count = 0;
        count.ref();
        auto currentCount = count.loadAcquire();

        QThreadPool::globalInstance()->start([this, currentCount] {
            auto roundedRect = roundedRectItemPtr->roundedRect();
            auto previewPixmap = pixmap.copy(roundedRect.rect.toRect());
            previewPixmap = QPixmap::fromImage(radiusImage(previewPixmap, roundedRect.xRadius));
            previewPixmap = previewPixmap.scaled(previewLabel->size(),
                                                 Qt::KeepAspectRatio,
                                                 Qt::SmoothTransformation);
            if (currentCount == count.loadAcquire()) {
                QMetaObject::invokeMethod(
                    q_ptr, [=] { previewLabel->setPixmap(previewPixmap); }, Qt::QueuedConnection);
            }
        });

        QThreadPool::globalInstance()->start([this, currentCount] {
            auto roundedRect = roundedRectItemPtr->roundedRect();
            QImage image(pixmap.size(), QImage::Format_RGBA8888_Premultiplied);
            image.fill(Qt::transparent);
            QPainterPath p1, p2;
            p1.addRect(pixmap.rect());
            p2.addRoundedRect(roundedRect.rect, roundedRect.xRadius, roundedRect.yRadius);
            p1 = p1.subtracted(p2);
            {
                QPainter painter(&image);
                painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
                painter.setOpacity(0.6);
                painter.fillPath(p1, QBrush(Qt::black));
            }
            if (currentCount == count.loadAcquire()) {
                QMetaObject::invokeMethod(
                    q_ptr,
                    [=] { imageView->pixmapItem()->setMaskImage(image); },
                    Qt::QueuedConnection);
            }
        });
    }

    RoundDialog *q_ptr;

    GraphicsView *imageView;

    QLabel *previewLabel;

    QGroupBox *rectGroupBox;
    QSpinBox *topLeftXSpinBox;
    QSpinBox *topLeftYSpinBox;
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;
    QSpinBox *radiusSpinBox;

    QString name;

    QScopedPointer<GraphicsRoundedRectItem> roundedRectItemPtr;
    QPixmap pixmap;
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

    RoundedRect roundedRect;
    roundedRect.rect = pixmap.rect().adjusted(10, 10, -10, -10);
    roundedRect.xRadius = roundedRect.yRadius = d_ptr->radiusSpinBox->value();
    d_ptr->setRoundedRect(roundedRect);
    if (!d_ptr->imageView->scene()->items().contains(d_ptr->roundedRectItemPtr.data())) {
        d_ptr->imageView->scene()->addItem(d_ptr->roundedRectItemPtr.data());
    }
    buildConnect2();
}

void RoundDialog::onSave()
{
    auto roundedRect = d_ptr->roundedRectItemPtr->roundedRect();
    auto pixmap = d_ptr->pixmap.copy(roundedRect.rect.toRect());
    pixmap = QPixmap::fromImage(radiusImage(pixmap, roundedRect.xRadius));

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

void RoundDialog::onRoundedRectChanged(const Graphics::RoundedRect &roundedRect)
{
    auto rectF = roundedRect.rect;

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
    d_ptr->updatePreview();
}

void RoundDialog::onTopLeftXChanged(int value)
{
    auto roundedRect = d_ptr->roundedRectItemPtr->roundedRect();
    QPointF point1 = roundedRect.rect.topLeft();
    point1.setX(value);
    roundedRect.rect = QRectF(point1, roundedRect.rect.bottomRight());
    d_ptr->setRoundedRect(roundedRect);
}

void RoundDialog::onTopLeftYChanged(int value)
{
    auto roundedRect = d_ptr->roundedRectItemPtr->roundedRect();
    QPointF point1 = roundedRect.rect.topLeft();
    point1.setY(value);
    roundedRect.rect = QRectF(point1, roundedRect.rect.bottomRight());
    d_ptr->setRoundedRect(roundedRect);
}

void RoundDialog::onWidthChanged(int value)
{
    auto roundedRect = d_ptr->roundedRectItemPtr->roundedRect();
    QPointF point1 = roundedRect.rect.bottomRight();
    point1.setX(value + roundedRect.rect.x());
    roundedRect.rect = QRectF(roundedRect.rect.topLeft(), point1);
    d_ptr->setRoundedRect(roundedRect);
}

void RoundDialog::onHeightChanged(int value)
{
    auto roundedRect = d_ptr->roundedRectItemPtr->roundedRect();
    QPointF point1 = roundedRect.rect.bottomRight();
    point1.setY(value + roundedRect.rect.y());
    roundedRect.rect = QRectF(roundedRect.rect.topLeft(), point1);
    d_ptr->setRoundedRect(roundedRect);
}

void RoundDialog::onRadiusChanged(int value)
{
    auto roundedRect = d_ptr->roundedRectItemPtr->roundedRect();
    roundedRect.xRadius = roundedRect.yRadius = value;
    d_ptr->setRoundedRect(roundedRect);
}

void RoundDialog::setupUI()
{
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(d_ptr->imageView);
    splitter->addWidget(toolWidget());
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({INT_MAX, 1});

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->addWidget(splitter);
}

void RoundDialog::buildConnect()
{
    connect(d_ptr->roundedRectItemPtr.data(),
            &Graphics::GraphicsRoundedRectItem::roundedRectChanged,
            this,
            &RoundDialog::onRoundedRectChanged);
    connect(d_ptr->radiusSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &RoundDialog::onRadiusChanged);
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
    auto *formLayout = new QFormLayout(d_ptr->rectGroupBox);
    formLayout->addRow(tr("TopLeft X:"), d_ptr->topLeftXSpinBox);
    formLayout->addRow(tr("TopLeft Y:"), d_ptr->topLeftYSpinBox);
    formLayout->addRow(tr("Width:"), d_ptr->widthSpinBox);
    formLayout->addRow(tr("Height:"), d_ptr->heightSpinBox);
    formLayout->addRow(tr("Radius:"), d_ptr->radiusSpinBox);

    auto *saveButton = new QPushButton(tr("Save"), this);
    connect(saveButton, &QPushButton::clicked, this, &RoundDialog::onSave);

    auto *toolWidget = new QWidget(this);
    toolWidget->setMaximumWidth(250);
    auto *toolLayout = new QVBoxLayout(toolWidget);
    toolLayout->addWidget(new QLabel(tr("Preview:"), this));
    toolLayout->addWidget(d_ptr->previewLabel);
    toolLayout->addWidget(d_ptr->rectGroupBox);
    toolLayout->addWidget(saveButton);
    toolLayout->addStretch();
    return toolWidget;
}

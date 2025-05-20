#include "imageviewer.h"
#include "maskdialog.h"
#include "rounddialog.hpp"

#include <examples/common/imagelistmodel.h>
#include <graphics/imageview.h>
#include <utils/utils.h>

#include <QtWidgets>

class ImageViewer::ImageViewerPrivate
{
public:
    explicit ImageViewerPrivate(ImageViewer *q)
        : q_ptr(q)
    {
        imageView = new Graphics::ImageView(q_ptr);
        imageViewFormat = new Graphics::ImageView(q_ptr);

        formatBox = new QComboBox(q_ptr);
        colorBox = new QComboBox(q_ptr);

        const auto imageFormat = QMetaEnum::fromType<QImage::Format>();
        for (int i = 0; i < imageFormat.keyCount(); i++) {
            formatBox->addItem(imageFormat.key(i), imageFormat.value(i));
        }
        const auto Conversionflags = QMetaEnum::fromType<Qt::ImageConversionFlags>();
        for (int i = 0; i < Conversionflags.keyCount(); i++) {
            colorBox->addItem(Conversionflags.key(i), Conversionflags.value(i));
        }
    }

    ImageViewer *q_ptr;

    Graphics::ImageView *imageView;
    Graphics::ImageView *imageViewFormat;

    QComboBox *formatBox;
    QComboBox *colorBox;
};

ImageViewer::ImageViewer(QWidget *parent)
    : Viewer(parent)
    , d_ptr(new ImageViewerPrivate(this))
{
    setupUI();
    buildConnect();
}

ImageViewer::~ImageViewer() {}

void ImageViewer::onOpenImage()
{
    const auto filename = openImage();
    if (filename.isEmpty()) {
        return;
    }
    d_ptr->imageView->createScene(filename);
}

void ImageViewer::onMaskImage()
{
    auto pixmap = d_ptr->imageView->pixmap();
    if (pixmap.isNull()) {
        return;
    }
    MaskDialog dialog(this);
    dialog.setPixmap(pixmap);
    dialog.setImageName(QFileInfo(m_urlLabel->text().trimmed()).fileName());
    dialog.exec();
}

void ImageViewer::onRoundImage()
{
    auto pixmap = d_ptr->imageView->pixmap();
    if (pixmap.isNull()) {
        return;
    }
    RoundDialog dialog(this);
    dialog.setPixmap(pixmap);
    dialog.setImageName(QFileInfo(m_urlLabel->text().trimmed()).fileName());
    dialog.exec();
}

void ImageViewer::onChangedImage(int index)
{
    d_ptr->imageView->createScene(m_thumbnailList.at(index).fileInfo().absoluteFilePath());
}

void ImageViewer::onFormatChecked(bool state)
{
    d_ptr->formatBox->setVisible(state);
    d_ptr->imageViewFormat->setVisible(state);
    d_ptr->colorBox->setVisible(state);
}

void ImageViewer::onFormatChanged(const QString &)
{
    auto format = QImage::Format(d_ptr->formatBox->currentData().toInt());
    auto flags = Qt::ImageConversionFlags(d_ptr->colorBox->currentData().toInt());

    auto image = d_ptr->imageView->pixmap().toImage();
    auto pixmap = QPixmap::fromImage(image.convertToFormat(format, flags));
    if (pixmap.isNull()) {
        QMessageBox::warning(this, tr("WARNING"), tr("Format Conversion Failed!"));
        return;
    }
    d_ptr->imageViewFormat->setPixmap(pixmap);
}

void ImageViewer::setupUI()
{
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(d_ptr->imageView);
    splitter->addWidget(d_ptr->imageViewFormat);
    splitter->addWidget(toolWidget());
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 1);
    splitter->setSizes({INT_MAX, INT_MAX, 1});

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->addWidget(splitter);
    layout->addWidget(m_imageListView);

    d_ptr->formatBox->hide();
    d_ptr->imageViewFormat->hide();
    d_ptr->colorBox->hide();
}

QWidget *ImageViewer::toolWidget()
{
    auto *maskImageButton = new QPushButton(tr("Mask Picture"), this);
    connect(maskImageButton, &QPushButton::clicked, this, &ImageViewer::onMaskImage);

    auto *roundImageButton = new QPushButton(tr("Round Picture"), this);
    connect(roundImageButton, &QPushButton::clicked, this, &ImageViewer::onRoundImage);

    auto *formatBox = new QCheckBox(tr("Format"), this);
    connect(formatBox, &QCheckBox::clicked, this, &ImageViewer::onFormatChecked);
    auto *formatLayout = new QHBoxLayout;
    formatLayout->addWidget(formatBox);
    formatLayout->addWidget(d_ptr->formatBox);
    formatLayout->addWidget(d_ptr->colorBox);

    auto *widget = new QWidget(this);
    widget->setMaximumWidth(300);
    auto *rightLayout = new QVBoxLayout(widget);
    rightLayout->addWidget(m_openButton);
    rightLayout->addWidget(m_infoBox);
    rightLayout->addLayout(formatLayout);
    rightLayout->addWidget(maskImageButton);
    rightLayout->addWidget(roundImageButton);
    rightLayout->addStretch();

    return widget;
}

void ImageViewer::buildConnect()
{
    connect(m_openButton, &QPushButton::clicked, this, &ImageViewer::onOpenImage);

    connect(d_ptr->imageView,
            &Graphics::ImageView::scaleFactorChanged,
            this,
            &ImageViewer::onScaleFactorChanged);
    connect(d_ptr->imageView,
            &Graphics::ImageView::imageSizeChanged,
            this,
            &ImageViewer::onImageSizeChanged);
    connect(d_ptr->imageView,
            &Graphics::ImageView::imageUrlChanged,
            this,
            &ImageViewer::onImageChanged);
    connect(m_imageListView, &ImageListView::changeItem, this, &ImageViewer::onChangedImage);
    connect(d_ptr->formatBox, &QComboBox::currentTextChanged, this, &ImageViewer::onFormatChanged);
    connect(d_ptr->colorBox, &QComboBox::currentTextChanged, this, &ImageViewer::onFormatChanged);
}

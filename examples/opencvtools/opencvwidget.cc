#include "opencvwidget.hpp"

#include <examples/common/imagelistmodel.h>
#include <graphics/graphicsview.hpp>
#include <utils/utils.hpp>
#include <qopencv/opencvutils.hpp>
#include <qopencv/qopencv.hpp>

#include <QtWidgets>

#include <opencv2/core/utility.hpp>

class OpenCVWidget::OpenCVWidgetPrivate
{
public:
    explicit OpenCVWidgetPrivate(OpenCVWidget *q)
        : q_ptr(q)
    {
        imageView = new Graphics::GraphicsView(q_ptr);

        toolLayout = new QVBoxLayout;

        typeComboBox = new QComboBox(q_ptr);
        auto metaEnum = QMetaEnum::fromType<OpenCVUtils::OpenCVOBject::AlgorithmType>();
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            typeComboBox->addItem(metaEnum.key(i), metaEnum.value(i));
        }
        algorithmComboBox = new QComboBox(q_ptr);

        applyButton = new QToolButton(q_ptr);
        auto sizePolicy = applyButton->sizePolicy();
        sizePolicy.setHorizontalPolicy(QSizePolicy ::Preferred);
        applyButton->setSizePolicy(sizePolicy);
        applyButton->setText(OpenCVWidget::tr("Apply"));

        originalButton = new QToolButton(q_ptr);
        originalButton->setSizePolicy(sizePolicy);
        originalButton->setText(OpenCVWidget::tr("Original Image"));
    }

    OpenCVWidget *q_ptr;

    Graphics::GraphicsView *imageView;

    QVBoxLayout *toolLayout;
    QComboBox *typeComboBox;
    QComboBox *algorithmComboBox;
    QToolButton *applyButton;
    QScopedPointer<OpenCVUtils::OpenCVOBject> currentOpenCVOBjectPtr;

    QImage image;
    QToolButton *originalButton;
};

OpenCVWidget::OpenCVWidget(QWidget *parent)
    : Viewer{parent}
    , d_ptr{new OpenCVWidgetPrivate(this)}
{
    qInfo().noquote() << cv::getBuildInformation();

    setupUI();
    buildConnect();

    QMetaObject::invokeMethod(this, &OpenCVWidget::onTypeChanged, Qt::QueuedConnection);
}

OpenCVWidget::~OpenCVWidget() {}

void OpenCVWidget::onOpenImage()
{
    const auto filename = openImage();
    if (filename.isEmpty()) {
        return;
    }
    d_ptr->imageView->createScene(filename);
    d_ptr->image = {};
}

void OpenCVWidget::onChangedImage(int index)
{
    d_ptr->imageView->createScene(m_thumbnailList.at(index).fileInfo().absoluteFilePath());
    d_ptr->image = {};
}

void OpenCVWidget::onShowOriginalImage()
{
    if (d_ptr->image.isNull()) {
        return;
    }

    d_ptr->imageView->setPixmap(QPixmap::fromImage(d_ptr->image));
}

void OpenCVWidget::onTypeChanged()
{
    auto type = static_cast<OpenCVUtils::OpenCVOBject::AlgorithmType>(
        d_ptr->typeComboBox->currentData().toInt());
    switch (type) {
    case OpenCVUtils::OpenCVOBject::AlgorithmType::Enhancement: {
        d_ptr->algorithmComboBox->clear();
        auto metaEnum = QMetaEnum::fromType<OpenCVUtils::Enhancement::Type>();
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            d_ptr->algorithmComboBox->addItem(metaEnum.key(i), metaEnum.value(i));
        }
        break;
    }
    case OpenCVUtils::OpenCVOBject::AlgorithmType::Filter: {
        d_ptr->algorithmComboBox->clear();
        auto metaEnum = QMetaEnum::fromType<OpenCVUtils::Filter::Type>();
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            d_ptr->algorithmComboBox->addItem(metaEnum.key(i), metaEnum.value(i));
        }
        break;
    }
    case OpenCVUtils::OpenCVOBject::AlgorithmType::EdgeDetection: {
        d_ptr->algorithmComboBox->clear();
        auto metaEnum = QMetaEnum::fromType<OpenCVUtils::EdgeDetection::Type>();
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            d_ptr->algorithmComboBox->addItem(metaEnum.key(i), metaEnum.value(i));
        }
        break;
    }
    case OpenCVUtils::OpenCVOBject::AlgorithmType::Segmentation: {
        d_ptr->algorithmComboBox->clear();
        auto metaEnum = QMetaEnum::fromType<OpenCVUtils::Segmentation::Type>();
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            d_ptr->algorithmComboBox->addItem(metaEnum.key(i), metaEnum.value(i));
        }
        break;
    }
    default: break;
    }
}

void OpenCVWidget::onAlgorithmChanged()
{
    auto type = static_cast<OpenCVUtils::OpenCVOBject::AlgorithmType>(
        d_ptr->typeComboBox->currentData().toInt());
    switch (type) {
    case OpenCVUtils::OpenCVOBject::AlgorithmType::Enhancement: {
        auto algo = static_cast<OpenCVUtils::Enhancement::Type>(
            d_ptr->algorithmComboBox->currentData().toInt());
        d_ptr->currentOpenCVOBjectPtr.reset(OpenCVUtils::createOpenCVOBject(algo));
        break;
    }
    case OpenCVUtils::OpenCVOBject::AlgorithmType::Filter: {
        auto algo = static_cast<OpenCVUtils::Filter::Type>(
            d_ptr->algorithmComboBox->currentData().toInt());
        d_ptr->currentOpenCVOBjectPtr.reset(OpenCVUtils::createOpenCVOBject(algo));
        break;
    }
    case OpenCVUtils::OpenCVOBject::AlgorithmType::EdgeDetection: {
        auto algo = static_cast<OpenCVUtils::EdgeDetection::Type>(
            d_ptr->algorithmComboBox->currentData().toInt());
        d_ptr->currentOpenCVOBjectPtr.reset(OpenCVUtils::createOpenCVOBject(algo));
        break;
    }
    case OpenCVUtils::OpenCVOBject::AlgorithmType::Segmentation: {
        auto algo = static_cast<OpenCVUtils::Segmentation::Type>(
            d_ptr->algorithmComboBox->currentData().toInt());
        d_ptr->currentOpenCVOBjectPtr.reset(OpenCVUtils::createOpenCVOBject(algo));
        break;
    }
    default: break;
    }

    if (d_ptr->currentOpenCVOBjectPtr.isNull()) {
        return;
    }
    d_ptr->toolLayout->insertWidget(d_ptr->toolLayout->indexOf(d_ptr->applyButton),
                                    d_ptr->currentOpenCVOBjectPtr->paramWidget());
}

void OpenCVWidget::onApply()
{
    d_ptr->applyButton->setEnabled(false);
    d_ptr->applyButton->setText(OpenCVWidget::tr("Applying..."));
    auto enabled = qScopeGuard([this]() {
        d_ptr->applyButton->setEnabled(true);
        d_ptr->applyButton->setText(OpenCVWidget::tr("Apply"));
    });

    if (d_ptr->currentOpenCVOBjectPtr.isNull() || !d_ptr->currentOpenCVOBjectPtr->canApply()) {
        QMessageBox::warning(this,
                             OpenCVWidget::tr("Warning"),
                             OpenCVWidget::tr("Please select a valid algorithm first!"));
        return;
    }

    if (d_ptr->image.isNull()) {
        d_ptr->image = d_ptr->imageView->pixmap().toImage();
    }
    if (d_ptr->image.isNull()) {
        QMessageBox::warning(this,
                             OpenCVWidget::tr("Warning"),
                             OpenCVWidget::tr("Please open an image first!"));
        return;
    }
    auto mat = Utils::asynchronous<cv::Mat>(
        [this]() -> cv::Mat { return OpenCVUtils::qImageToMat(d_ptr->image); });
    mat = d_ptr->currentOpenCVOBjectPtr->apply(mat);
    auto pixmap = Utils::asynchronous<QPixmap>(
        [mat]() -> QPixmap { return QPixmap::fromImage(OpenCVUtils::matToQImage(mat)); });
    d_ptr->imageView->setPixmap(pixmap);
}

void OpenCVWidget::setupUI()
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
    layout->addWidget(m_imageListView);
}

auto OpenCVWidget::toolWidget() -> QWidget *
{
    auto *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(tr("Type:"), this), 0, 0);
    gridLayout->addWidget(d_ptr->typeComboBox, 0, 1);
    gridLayout->addWidget(new QLabel(tr("Algorithm:"), this), 1, 0);
    gridLayout->addWidget(d_ptr->algorithmComboBox, 1, 1);

    d_ptr->toolLayout->addWidget(m_openButton);
    d_ptr->toolLayout->addWidget(m_infoBox);
    d_ptr->toolLayout->addStretch();
    d_ptr->toolLayout->addWidget(d_ptr->originalButton);
    d_ptr->toolLayout->addLayout(gridLayout);
    d_ptr->toolLayout->addWidget(d_ptr->applyButton);
    d_ptr->toolLayout->addStretch();

    auto *widget = new QWidget(this);
    widget->setMaximumWidth(300);
    widget->setLayout(d_ptr->toolLayout);

    return widget;
}

void OpenCVWidget::buildConnect()
{
    connect(m_openButton, &QPushButton::clicked, this, &OpenCVWidget::onOpenImage);

    connect(d_ptr->imageView,
            &Graphics::GraphicsView::scaleFactorChanged,
            this,
            &OpenCVWidget::onScaleFactorChanged);
    connect(d_ptr->imageView,
            &Graphics::GraphicsView::imageSizeChanged,
            this,
            &OpenCVWidget::onImageSizeChanged);
    connect(d_ptr->imageView,
            &Graphics::GraphicsView::imageUrlChanged,
            this,
            &OpenCVWidget::onImageChanged);
    connect(m_imageListView, &ImageListView::changeItem, this, &OpenCVWidget::onChangedImage);

    connect(d_ptr->typeComboBox,
            &QComboBox::currentIndexChanged,
            this,
            &OpenCVWidget::onTypeChanged);
    connect(d_ptr->algorithmComboBox,
            &QComboBox::currentIndexChanged,
            this,
            &OpenCVWidget::onAlgorithmChanged);

    connect(d_ptr->originalButton, &QToolButton::clicked, this, &OpenCVWidget::onShowOriginalImage);
    connect(d_ptr->applyButton, &QToolButton::clicked, this, &OpenCVWidget::onApply);
}

#include "icoconverterwidget.hpp"
#include "sizeselectorwidget.hpp"

#include <graphics/graphicsview.hpp>
#include <utils/icowriter.hpp>
#include <utils/utils.hpp>

#include <QtWidgets>

class IcoConverterWidget::IcoConverterWidgetPrivate
{
public:
    explicit IcoConverterWidgetPrivate(IcoConverterWidget *q)
        : q_ptr(q)
    {
        imageView = new Graphics::GraphicsView(q_ptr);

        sizeSelectorWidget = new SizeSelectorWidget(q_ptr);
        sizeSelectorWidget->setSizes(Utils::defaultIcoSizes);

        colorLabel = new QLabel(q_ptr);
        colorLabel->setFrameShape(QFrame::Box);
        colorLabel->setAutoFillBackground(true);
        updateColorLabel();

        pathLineEdit = new QLineEdit(q_ptr);
        auto path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        if (path.isEmpty()) {
            path = QDir::homePath();
        }
        pathLineEdit->setText(path);

        convertButton = new QPushButton(IcoConverterWidget::tr("Convert"), q_ptr);
    }
    ~IcoConverterWidgetPrivate() {}

    void updateColorLabel()
    {
        auto pal = colorLabel->palette();
        pal.setColor(QPalette::Window, color);
        pal.setColor(QPalette::WindowText, color.lightness() > 128 ? Qt::black : Qt::white);
        colorLabel->setPalette(pal);
        colorLabel->setText(color.name(QColor::HexArgb));

        colorLabel->update();
    }

    IcoConverterWidget *q_ptr;

    Graphics::GraphicsView *imageView;
    SizeSelectorWidget *sizeSelectorWidget;
    QLabel *colorLabel;
    QColor color = Qt::transparent;
    QLineEdit *pathLineEdit;
    QPushButton *convertButton;
};

IcoConverterWidget::IcoConverterWidget(QWidget *parent)
    : Viewer(parent)
    , d_ptr(new IcoConverterWidgetPrivate(this))
{
    setupUI();
    buildConnect();
}

IcoConverterWidget::~IcoConverterWidget() {}

void IcoConverterWidget::onOpenImage()
{
    const auto filename = openImage();
    if (filename.isEmpty()) {
        return;
    }
    d_ptr->imageView->createScene(filename);
}

void IcoConverterWidget::onChangedImage(int index)
{
    if (index < 0 || index >= m_thumbnailList.size()) {
        return;
    }
    const auto data = m_thumbnailList.at(index);
    d_ptr->imageView->createScene(data.fileInfo().absoluteFilePath());
}

void IcoConverterWidget::onPickSquareColor()
{
    auto color = QColorDialog::getColor(d_ptr->color, this, tr("Select Background Color"));
    if (!color.isValid()) {
        return;
    }
    d_ptr->color = color;
    d_ptr->updateColorLabel();
}

void IcoConverterWidget::onBrowse()
{
    auto dir = QFileDialog::getExistingDirectory(this,
                                                 tr("Select Directory"),
                                                 d_ptr->pathLineEdit->text());
    if (dir.isEmpty()) {
        return;
    }
    d_ptr->pathLineEdit->setText(dir);
}

void IcoConverterWidget::onConvert()
{
    auto path = d_ptr->pathLineEdit->text();
    if (path.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a directory."));
        return;
    }
    if (!QDir(path).exists()) {
        QMessageBox::warning(this, tr("Warning"), tr("Directory not exist."));
        return;
    }
    auto image = d_ptr->imageView->pixmap().toImage();
    if (image.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("No image loaded."));
        return;
    }

    d_ptr->convertButton->setEnabled(false);

    auto filename = QString("%1_%2.ico")
                        .arg(QFileInfo(m_urlLabel->text().trimmed()).baseName(),
                             QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    path = QDir(path).filePath(filename);
    auto sizes = d_ptr->sizeSelectorWidget->sizes();

    Utils::asynchronous<bool>([image, path, sizes, color = d_ptr->color]() {
        return Utils::writeIco(image, path, color, sizes);
    });
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absolutePath()));

    d_ptr->convertButton->setEnabled(true);
}

void IcoConverterWidget::setupUI()
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

auto IcoConverterWidget::toolWidget() -> QWidget *
{
    auto *pickColorButton = new QPushButton(tr("Pick Color"), this);
    connect(pickColorButton, &QPushButton::clicked, this, &IcoConverterWidget::onPickSquareColor);

    auto *browserButton = new QPushButton(tr("Browse"), this);
    connect(browserButton, &QPushButton::clicked, this, &IcoConverterWidget::onBrowse);

    auto *saveBox = new QGroupBox(tr("Save Options"), this);
    auto *saveLayout = new QVBoxLayout(saveBox);
    saveLayout->addWidget(d_ptr->sizeSelectorWidget);
    saveLayout->addWidget(new QLabel(tr("Background Color:"), saveBox));
    saveLayout->addWidget(d_ptr->colorLabel);
    saveLayout->addWidget(pickColorButton);
    saveLayout->addWidget(new QLabel(tr("Save Path:"), saveBox));
    saveLayout->addWidget(d_ptr->pathLineEdit);
    saveLayout->addWidget(browserButton);

    auto *widget = new QWidget(this);
    widget->setMaximumWidth(300);
    auto *rightLayout = new QVBoxLayout(widget);
    rightLayout->addWidget(m_openButton);
    rightLayout->addWidget(m_infoBox);
    rightLayout->addStretch();
    rightLayout->addWidget(saveBox);
    rightLayout->addWidget(d_ptr->convertButton);
    rightLayout->addStretch();

    return widget;
}

void IcoConverterWidget::buildConnect()
{
    connect(m_openButton, &QPushButton::clicked, this, &IcoConverterWidget::onOpenImage);

    connect(d_ptr->imageView,
            &Graphics::GraphicsView::scaleFactorChanged,
            this,
            &IcoConverterWidget::onScaleFactorChanged);
    connect(d_ptr->imageView,
            &Graphics::GraphicsView::imageSizeChanged,
            this,
            &IcoConverterWidget::onImageSizeChanged);
    connect(d_ptr->imageView,
            &Graphics::GraphicsView::imageUrlChanged,
            this,
            &IcoConverterWidget::onImageChanged);
    connect(m_imageListView, &ImageListView::changeItem, this, &IcoConverterWidget::onChangedImage);

    connect(d_ptr->convertButton, &QPushButton::clicked, this, &IcoConverterWidget::onConvert);
}

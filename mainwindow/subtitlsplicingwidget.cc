#include "subtitlsplicingwidget.hpp"
#include "sectionalsubtitlesview.hpp"

#include <QtWidgets>

class SubtitlSplicingWidget::SubtitlSplicingWidgetPrivate
{
public:
    SubtitlSplicingWidgetPrivate(QWidget *parnet)
        : owner(parnet)
    {
        listWidget = new QListWidget(owner);
        listWidget->setSpacing(20);
        imageView = new Graphics::ImageView(owner);
    }

    QWidget *owner;

    QListWidget *listWidget;
    Graphics::ImageView *imageView;
};

SubtitlSplicingWidget::SubtitlSplicingWidget(QWidget *parent)
    : QWidget{parent}
    , d_ptr(new SubtitlSplicingWidgetPrivate(this))
{
    setupUI();
}

SubtitlSplicingWidget::~SubtitlSplicingWidget() {}

void SubtitlSplicingWidget::onOpenImage()
{
    QString imageFilters(tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.svg *.tiff *.webp *.icns "
                            "*.bitmap *.graymap *.pixmap *.tga *.xbitmap *.xpixmap)"));
    qDebug() << imageFilters;
    const QString path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                             .value(0, QDir::homePath());
    auto paths = QFileDialog::getOpenFileNames(this, tr("Open Image"), path, imageFilters);
    if (paths.isEmpty()) {
        return;
    }

    d_ptr->listWidget->clear();
    for (const auto &path : qAsConst(paths)) {
        auto item = new QListWidgetItem;
        item->setSizeHint(QSize(1, 300));
        auto view = new SectionalSubtitlesView(this);
        view->setImagePath(path);
        d_ptr->listWidget->addItem(item);
        d_ptr->listWidget->setItemWidget(item, view);
    }
}

void SubtitlSplicingWidget::onGenerated()
{
    auto button = qobject_cast<QPushButton *>(sender());
    if (!button) {
        return;
    }
    button->setEnabled(false);
    auto cleanup = qScopeGuard([=] { button->setEnabled(true); });

    QImage image;
    for (int i = 0; i < d_ptr->listWidget->count(); i++) {
        auto item = d_ptr->listWidget->item(i);
        auto widget = d_ptr->listWidget->itemWidget(item);
        auto view = qobject_cast<SectionalSubtitlesView *>(widget);
        if (i == 0) {
            image = view->image();
        } else {
            auto clipImage = view->clipImage();
            QImage tmpImage(image.width(),
                            image.height() + clipImage.height(),
                            QImage::Format_ARGB32);
            QPainter painter(&tmpImage);
            painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            painter.drawImage(image.rect(), image);
            auto rect = QRect(0, image.height(), image.width(), clipImage.height());
            painter.drawImage(rect, clipImage);

            image = tmpImage;
        }
    }
    d_ptr->imageView->setPixmap(QPixmap::fromImage(image));
}

void SubtitlSplicingWidget::setupUI()
{
    auto openButton = new QPushButton(tr("Open Image"), this);
    connect(openButton, &QPushButton::clicked, this, &SubtitlSplicingWidget::onOpenImage);
    auto generateButton = new QPushButton(tr("Generate"), this);
    connect(generateButton, &QPushButton::clicked, this, &SubtitlSplicingWidget::onGenerated);

    auto buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(generateButton);

    auto layout = new QHBoxLayout(this);
    layout->addWidget(d_ptr->listWidget);
    layout->addLayout(buttonLayout);
    layout->addWidget(d_ptr->imageView);
}

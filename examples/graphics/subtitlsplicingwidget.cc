#include "subtitlsplicingwidget.hpp"
#include "sectionalsubtitlesview.hpp"
#include "stretchparamssettingdailog.hpp"

#include <QtWidgets>

class GenerateTask : public QRunnable
{
public:
    GenerateTask(const QVector<StitchingImageInfo> &infos,
                 SubtitlSplicingWidget *subtitlSplicingWidget)
        : m_infos(infos)
        , m_subtitlSplicingWidgetPtr(subtitlSplicingWidget)
    {
        setAutoDelete(true);
    }

    void run() override
    {
        QImage image;
        for (const auto &info : std::as_const(m_infos)) {
            auto i = QImage(info.imagePath);
            if (image.isNull()) {
                auto clipRect = QRect(QPoint(0, 0), info.imageRect.bottomRight());
                image = i.copy(clipRect);
            } else {
                auto clipImage = i.copy(info.imageRect);
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
        if (m_subtitlSplicingWidgetPtr.isNull()) {
            return;
        }
        m_subtitlSplicingWidgetPtr->setImage(image);
    }

private:
    QVector<StitchingImageInfo> m_infos;
    QPointer<SubtitlSplicingWidget> m_subtitlSplicingWidgetPtr;
};

class SubtitlSplicingWidget::SubtitlSplicingWidgetPrivate
{
public:
    SubtitlSplicingWidgetPrivate(QWidget *parnet)
        : q_ptr(parnet)
    {
        listWidget = new QListWidget(q_ptr);
        listWidget->setSpacing(10);
        listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        imageView = new Graphics::ImageView(q_ptr);
        generateButton = new QPushButton(QObject::tr("Generate", "SubtitlSplicingWidget"), q_ptr);
    }

    QWidget *q_ptr;

    QListWidget *listWidget;
    Graphics::ImageView *imageView;
    QPushButton *generateButton;
};

SubtitlSplicingWidget::SubtitlSplicingWidget(QWidget *parent)
    : QWidget{parent}
    , d_ptr(new SubtitlSplicingWidgetPrivate(this))
{
    setupUI();
}

SubtitlSplicingWidget::~SubtitlSplicingWidget() {}

void SubtitlSplicingWidget::setImage(const QImage &image)
{
    QMetaObject::invokeMethod(
        this,
        [=] {
            d_ptr->imageView->setPixmap(QPixmap::fromImage(image));
            d_ptr->generateButton->setEnabled(true);
        },
        Qt::QueuedConnection);
}

void SubtitlSplicingWidget::onOpenImage()
{
    QString imageFilters(tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.svg *.tiff *.webp *.icns "
                            "*.bitmap *.graymap *.pixmap *.tga *.xbitmap *.xpixmap)"));
    //qDebug() << imageFilters;
    const QString path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                             .value(0, QDir::homePath());
    auto paths = QFileDialog::getOpenFileNames(this, tr("Open Image"), path, imageFilters);
    if (paths.isEmpty()) {
        return;
    }

    d_ptr->listWidget->clear();
    for (const auto &path : std::as_const(paths)) {
        auto item = new QListWidgetItem;
        item->setSizeHint(QSize(1, 300));
        auto view = new SectionalSubtitlesView(this);
        view->setImagePath(path);
        d_ptr->listWidget->addItem(item);
        d_ptr->listWidget->setItemWidget(item, view);

        auto index = d_ptr->listWidget->indexFromItem(item).row();
        view->setIndex(index, index != 0, index != (paths.size() - 1));
        buildViewConnnect(view);
    }
}

void SubtitlSplicingWidget::onGenerated()
{
    d_ptr->generateButton->setEnabled(false);

    QVector<StitchingImageInfo> infos;
    for (int i = 0; i < d_ptr->listWidget->count(); i++) {
        auto item = d_ptr->listWidget->item(i);
        auto widget = d_ptr->listWidget->itemWidget(item);
        auto view = qobject_cast<SectionalSubtitlesView *>(widget);
        auto info = view->info();
        infos.append(info);
    }

    QThreadPool::globalInstance()->start(new GenerateTask(infos, this));

    // QImage image;
    // for (int i = 0; i < d_ptr->listWidget->count(); i++) {
    //     auto item = d_ptr->listWidget->item(i);
    //     auto widget = d_ptr->listWidget->itemWidget(item);
    //     auto view = qobject_cast<SectionalSubtitlesView *>(widget);
    //     if (i == 0) {
    //         image = view->image();
    //     } else {
    //         auto clipImage = view->clipImage();
    //         QImage tmpImage(image.width(),
    //                         image.height() + clipImage.height(),
    //                         QImage::Format_ARGB32);
    //         QPainter painter(&tmpImage);
    //         painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    //         painter.drawImage(image.rect(), image);
    //         auto rect = QRect(0, image.height(), image.width(), clipImage.height());
    //         painter.drawImage(rect, clipImage);

    //         image = tmpImage;
    //     }
    // }
    // d_ptr->imageView->setPixmap(QPixmap::fromImage(image));
}

void SubtitlSplicingWidget::onSave()
{
    auto pixmap = d_ptr->imageView->pixmap();
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

    auto path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                    .value(0, QDir::homePath());
    auto time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    QString name = time + ".jpg"; // PNG 太大了
    path = path + "/" + name;
    auto filename = QFileDialog::getSaveFileName(this,
                                                 tr("Save Image"),
                                                 path,
                                                 tr("Images (*.png *.xpm *.jpg)"));
    if (filename.isEmpty()) {
        return;
    }
    pixmap = pixmap.scaled(params.size, params.mode, Qt::SmoothTransformation);
    qDebug() << pixmap.save(filename, nullptr, params.quality);
}

void SubtitlSplicingWidget::onUp(int index)
{
    auto size = d_ptr->listWidget->count();
    Q_ASSERT(index > 0 && index < size);

    auto item = d_ptr->listWidget->item(index);
    auto view = qobject_cast<SectionalSubtitlesView *>(d_ptr->listWidget->itemWidget(item));
    auto imagePath = view->imagePath();

    item = d_ptr->listWidget->takeItem(index);
    d_ptr->listWidget->insertItem(index - 1, item);
    view = new SectionalSubtitlesView(this);
    view->setImagePath(imagePath);
    view->setIndex(index - 1, (index - 1) != 0, (index - 1) != (size - 1));
    d_ptr->listWidget->setItemWidget(item, view);
    buildViewConnnect(view);

    // 更新交换Item的索引
    item = d_ptr->listWidget->item(index);
    view = qobject_cast<SectionalSubtitlesView *>(d_ptr->listWidget->itemWidget(item));
    view->setIndex(index, index != 0, index != (size - 1));
}

void SubtitlSplicingWidget::onDown(int index)
{
    onUp(index + 1);
}

void SubtitlSplicingWidget::onLine1Changed()
{
    auto view = qobject_cast<SectionalSubtitlesView *>(sender());
    if (!view) {
        return;
    }
    if (view->index() != 0) {
        return;
    }

    auto radoi = view->line1RatioOfHeight();
    for (int i = 1; i < d_ptr->listWidget->count(); i++) {
        auto item = d_ptr->listWidget->item(i);
        auto view = qobject_cast<SectionalSubtitlesView *>(d_ptr->listWidget->itemWidget(item));
        view->setLine1RatioOfHeight(radoi);
    }
}

void SubtitlSplicingWidget::onLine2Changed()
{
    auto view = qobject_cast<SectionalSubtitlesView *>(sender());
    if (!view) {
        return;
    }
    if (view->index() != 0) {
        return;
    }

    auto radoi = view->line2RatioOfHeight();
    for (int i = 1; i < d_ptr->listWidget->count(); i++) {
        auto item = d_ptr->listWidget->item(i);
        auto view = qobject_cast<SectionalSubtitlesView *>(d_ptr->listWidget->itemWidget(item));
        view->setLine2RatioOfHeight(radoi);
    }
}

void SubtitlSplicingWidget::setupUI()
{
    auto openButton = new QPushButton(tr("Open Image"), this);
    connect(openButton, &QPushButton::clicked, this, &SubtitlSplicingWidget::onOpenImage);
    auto saveButton = new QPushButton(tr("Save"), this);
    connect(saveButton, &QPushButton::clicked, this, &SubtitlSplicingWidget::onSave);
    connect(d_ptr->generateButton, &QPushButton::clicked, this, &SubtitlSplicingWidget::onGenerated);

    auto buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(d_ptr->generateButton);
    buttonLayout->addWidget(saveButton);

    auto layout = new QHBoxLayout(this);
    layout->addWidget(d_ptr->listWidget);
    layout->addLayout(buttonLayout);
    layout->addWidget(d_ptr->imageView);
}

void SubtitlSplicingWidget::buildViewConnnect(SectionalSubtitlesView *view)
{
    connect(view, &SectionalSubtitlesView::up, this, &SubtitlSplicingWidget::onUp);
    connect(view, &SectionalSubtitlesView::down, this, &SubtitlSplicingWidget::onDown);
    connect(view,
            &SectionalSubtitlesView::line1Changed,
            this,
            &SubtitlSplicingWidget::onLine1Changed);
    connect(view,
            &SectionalSubtitlesView::line2Changed,
            this,
            &SubtitlSplicingWidget::onLine2Changed);
}

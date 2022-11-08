#include "sectionalsubtitlesview.hpp"
#include "customlineitem.hpp"

#include <QtWidgets>

class LoadImageTask : public QRunnable
{
public:
    LoadImageTask(const QString &path, SectionalSubtitlesView *parent)
        : m_imagePath(path)
        , m_viewPtr(parent)
    {
        setAutoDelete(true);
    }
    ~LoadImageTask() {}

    void run() override
    {
        QImage image(m_imagePath);
        QSize widgetSize;
        if (m_viewPtr.isNull() || image.isNull()) {
            return;
        } else {
            widgetSize = m_viewPtr->size();
        }
        auto imageSize = image.size();
        image = image.scaled(widgetSize, Qt::KeepAspectRatio /*, Qt::SmoothTransformation*/);
        if (m_viewPtr.isNull()) {
            return;
        }
        m_viewPtr->setImage(image, imageSize);
    }

private:
    QString m_imagePath;
    QPointer<SectionalSubtitlesView> m_viewPtr;
};

class SectionalSubtitlesView::SectionalSubtitlesViewPrivate
{
public:
    SectionalSubtitlesViewPrivate(QWidget *parent)
        : owner(parent)
        , lineitem1Ptr(new CustomLineItem)
        , lineitem2Ptr(new CustomLineItem)
    {
        upButton = new QPushButton(QObject::tr("UP", "SectionalSubtitlesView"), owner);
        upButton->setCursor(Qt::PointingHandCursor);
        downButton = new QPushButton(QObject::tr("DOWN", "SectionalSubtitlesView"), owner);
        downButton->setCursor(Qt::PointingHandCursor);
    }
    ~SectionalSubtitlesViewPrivate() {}

    QWidget *owner;
    QScopedPointer<CustomLineItem> lineitem1Ptr;
    QScopedPointer<CustomLineItem> lineitem2Ptr;
    QString path;
    QSize imageSize;

    QPushButton *upButton;
    QPushButton *downButton;
    int index = 0;
};

SectionalSubtitlesView::SectionalSubtitlesView(QWidget *parent)
    : Graphics::ImageView(parent)
    , d_ptr(new SectionalSubtitlesViewPrivate(this))
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setupUI();
    buildConnect();
}

SectionalSubtitlesView::~SectionalSubtitlesView() {}

void SectionalSubtitlesView::setIndex(int index, bool showUpButton, bool showDownButton)
{
    d_ptr->index = index;
    d_ptr->upButton->setVisible(showUpButton);
    d_ptr->downButton->setVisible(showDownButton);
}

int SectionalSubtitlesView::index() const
{
    return d_ptr->index;
}

void SectionalSubtitlesView::setImagePath(const QString &path)
{
    QThreadPool::globalInstance()->start(new LoadImageTask(path, this));
    d_ptr->path = path;
}

QString SectionalSubtitlesView::imagePath() const
{
    return d_ptr->path;
}

void SectionalSubtitlesView::setImage(const QImage &image, const QSize &size)
{
    d_ptr->imageSize = size;
    QMetaObject::invokeMethod(
        this,
        [=] {
            setPixmap(QPixmap::fromImage(image));
            auto rect = sceneRect();
            auto y = rect.height() / 5.0 * 4;
            d_ptr->lineitem1Ptr->setLine(QLineF(1, y, rect.width() - 1, y));
            d_ptr->lineitem2Ptr->setLine(
                QLineF(1, rect.height() - 2, rect.width() - 1, rect.height() - 2));
            scene()->addItem(d_ptr->lineitem1Ptr.data());
            scene()->addItem(d_ptr->lineitem2Ptr.data());

            QMetaObject::invokeMethod(
                this, [=] { fitToScreen(); }, Qt::QueuedConnection);
        },
        Qt::QueuedConnection);
}

QImage SectionalSubtitlesView::image() const
{
    return QImage(d_ptr->path);
    //return pixmap().toImage();
}

QImage SectionalSubtitlesView::clipImage() const
{
    auto y1 = line1RatioOfHeight() * d_ptr->imageSize.height();
    auto y2 = line2RatioOfHeight() * d_ptr->imageSize.height();
    if (y1 > y2) {
        std::swap(y1, y2);
    }
    auto image = this->image();
    image = image.copy(QRect(QPoint(0, y1), QPoint(d_ptr->imageSize.width(), y2)));
    return image;
}

StitchingImageInfo SectionalSubtitlesView::info() const
{
    auto y1 = line1RatioOfHeight() * d_ptr->imageSize.height();
    auto y2 = line2RatioOfHeight() * d_ptr->imageSize.height();
    if (y1 > y2) {
        std::swap(y1, y2);
    }
    auto rect = QRect(QPoint(0, y1), QPoint(d_ptr->imageSize.width(), y2));
    return {d_ptr->path, rect};
}

double SectionalSubtitlesView::line1RatioOfHeight() const
{
    return (d_ptr->lineitem1Ptr->line().y1() / pixmap().size().height());
}

void SectionalSubtitlesView::setLine1RatioOfHeight(double value)
{
    auto line = d_ptr->lineitem1Ptr->line();
    setLineHeightORatio(line, value);
    d_ptr->lineitem1Ptr->setLine(line);
}

double SectionalSubtitlesView::line2RatioOfHeight() const
{
    return (d_ptr->lineitem2Ptr->line().y1() / pixmap().size().height());
}

void SectionalSubtitlesView::setLine2RatioOfHeight(double value)
{
    auto line = d_ptr->lineitem2Ptr->line();
    setLineHeightORatio(line, value);
    d_ptr->lineitem2Ptr->setLine(line);
}

void SectionalSubtitlesView::resizeEvent(QResizeEvent *event)
{
    Graphics::ImageView::resizeEvent(event);
    QMetaObject::invokeMethod(
        this, [=] { fitToScreen(); }, Qt::QueuedConnection);
}

void SectionalSubtitlesView::setupUI()
{
    auto buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(QMargins());
    buttonLayout->addStretch();
    buttonLayout->addWidget(d_ptr->upButton);
    buttonLayout->addWidget(d_ptr->downButton);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(QMargins());
    layout->addLayout(buttonLayout);
    layout->addStretch();
}

void SectionalSubtitlesView::buildConnect()
{
    connect(d_ptr->upButton, &QPushButton::clicked, this, [this] { emit up(d_ptr->index); });
    connect(d_ptr->downButton, &QPushButton::clicked, this, [this] { emit down(d_ptr->index); });

    connect(d_ptr->lineitem1Ptr.data(),
            &Graphics::GraphicsLineItem::lineChnaged,
            this,
            &SectionalSubtitlesView::line1Changed);
    connect(d_ptr->lineitem2Ptr.data(),
            &Graphics::GraphicsLineItem::lineChnaged,
            this,
            &SectionalSubtitlesView::line2Changed);
}

void SectionalSubtitlesView::setLineHeightORatio(QLineF &line, double radio)
{
    Q_ASSERT(radio >= 0 && radio <= 1);
    auto y = pixmap().size().height() * radio;

    auto p1 = line.p1();
    auto p2 = line.p2();
    p1.setY(y);
    p2.setY(y);
    line = QLineF(p1, p2);
}

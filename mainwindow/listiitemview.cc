#include "listiitemview.hpp"

#include <QtWidgets>

class LoadImageTask : public QRunnable
{
public:
    LoadImageTask(const QString &path, ListItemView *parent)
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
    QPointer<ListItemView> m_viewPtr;
};

class ListItemView::ListItemViewPrivate
{
public:
    ListItemViewPrivate(QWidget *parent)
        : owner(parent)
    {
        upButton = new QPushButton(QObject::tr("UP", "ListItemView"), owner);
        upButton->setCursor(Qt::PointingHandCursor);
        downButton = new QPushButton(QObject::tr("DOWN", "ListItemView"), owner);
        downButton->setCursor(Qt::PointingHandCursor);
    }

    QWidget *owner;
    QString path;
    QSize imageSize;

    QPushButton *upButton;
    QPushButton *downButton;
    int index = 0;
};

ListItemView::ListItemView(QWidget *parent)
    : Graphics::ImageView(parent)
    , d_ptr(new ListItemViewPrivate(this))
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setupUI();
    buildConnect();
}

ListItemView::~ListItemView() {}

void ListItemView::setIndex(int index, bool showUpButton, bool showDownButton)
{
    d_ptr->index = index;
    d_ptr->upButton->setVisible(showUpButton);
    d_ptr->downButton->setVisible(showDownButton);
}

int ListItemView::index() const
{
    return d_ptr->index;
}

void ListItemView::setImagePath(const QString &path)
{
    QThreadPool::globalInstance()->start(new LoadImageTask(path, this));
    d_ptr->path = path;
}

QString ListItemView::imagePath() const
{
    return d_ptr->path;
}

void ListItemView::setImage(const QImage &image, const QSize &size)
{
    d_ptr->imageSize = size;
    QMetaObject::invokeMethod(
        this,
        [=] {
            setPixmap(QPixmap::fromImage(image));
            fitToScreen();
            setImageAfter();
        },
        Qt::QueuedConnection);
}

void ListItemView::setImageAfter() {}

QImage ListItemView::image() const
{
    return QImage(d_ptr->path);
}

QSize ListItemView::realImageSize() const
{
    return d_ptr->imageSize;
}

void ListItemView::resizeEvent(QResizeEvent *event)
{
    Graphics::ImageView::resizeEvent(event);
    QMetaObject::invokeMethod(this, &ListItemView::fitToScreen, Qt::QueuedConnection);
}

void ListItemView::setupUI()
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

void ListItemView::buildConnect()
{
    connect(d_ptr->upButton, &QPushButton::clicked, this, [this] { emit up(d_ptr->index); });
    connect(d_ptr->downButton, &QPushButton::clicked, this, [this] { emit down(d_ptr->index); });
}

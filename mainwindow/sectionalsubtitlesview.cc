#include "sectionalsubtitlesview.hpp"
#include "customlineitem.hpp"

#include <QPointer>
#include <QRunnable>
#include <QThreadPool>

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
        image = image.scaled(widgetSize, Qt::KeepAspectRatio /*, Qt::SmoothTransformation*/);
        if (m_viewPtr.isNull()) {
            return;
        }
        m_viewPtr->setImage(image);
    }

private:
    QString m_imagePath;
    QPointer<SectionalSubtitlesView> m_viewPtr;
};

struct SectionalSubtitlesView::SectionalSubtitlesViewPrivate
{
    SectionalSubtitlesViewPrivate()
        : lineitem1Ptr(new CustomLineItem)
        , lineitem2Ptr(new CustomLineItem)
    {}
    ~SectionalSubtitlesViewPrivate() {}

    QScopedPointer<CustomLineItem> lineitem1Ptr;
    QScopedPointer<CustomLineItem> lineitem2Ptr;
    QString path;
};

SectionalSubtitlesView::SectionalSubtitlesView(QWidget *parent)
    : Graphics::ImageView(parent)
    , d_ptr(new SectionalSubtitlesViewPrivate)
{}

SectionalSubtitlesView::~SectionalSubtitlesView() {}

void SectionalSubtitlesView::setImagePath(const QString &path)
{
    QThreadPool::globalInstance()->start(new LoadImageTask(path, this));
    d_ptr->path = path;
}

void SectionalSubtitlesView::setImage(const QImage &image)
{
    QMetaObject::invokeMethod(
        this,
        [=] {
            setPixmap(QPixmap::fromImage(image));
            update();
            auto rect = sceneRect();
            // fix me Cannot be set directly
            d_ptr->lineitem1Ptr->setLine(QLineF(0, rect.height(), rect.width(), 0));
            d_ptr->lineitem2Ptr->setLine(QLineF(0, rect.height(), rect.width(), 0));
            scene()->addItem(d_ptr->lineitem1Ptr.data());
            scene()->addItem(d_ptr->lineitem2Ptr.data());
            QMetaObject::invokeMethod(
                this,
                [=] {
                    auto y = rect.height() / 5.0 * 4;
                    d_ptr->lineitem1Ptr->setLine(QLineF(1, y, rect.width() - 1, y));
                    d_ptr->lineitem2Ptr->setLine(
                        QLineF(1, rect.height() - 2, rect.width() - 1, rect.height() - 2));
                },
                Qt::QueuedConnection);
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
    auto y1 = d_ptr->lineitem1Ptr->line().y1();
    auto y2 = d_ptr->lineitem2Ptr->line().y1();
    auto image = this->image();
    auto realSize = image.size();
    auto showSize = pixmap().size();
    y1 = y1 * 1.0 / showSize.height() * realSize.height();
    y2 = y2 * 1.0 / showSize.height() * realSize.height();
    image = image.copy(QRect(QPoint(0, y1), QPoint(realSize.width(), y2)));
    return image;
}

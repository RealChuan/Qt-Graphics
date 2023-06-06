#include "viewer.hpp"
#include "imagelistmodel.h"

#include <QDir>
#include <QPointer>

Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
{}

Viewer::~Viewer() {}

class ImageLoadRunnable::ImageLoadRunnablePrivate
{
public:
    ImageLoadRunnablePrivate(ImageLoadRunnable *q)
        : q_ptr(q)
    {}

    ImageLoadRunnable *q_ptr;

    QPointer<Viewer> viewPtr;
    QString fileUrl;
    qint64 taskCount;
};

ImageLoadRunnable::ImageLoadRunnable(const QString &fileUrl, Viewer *view, qint64 taskCount)
    : d_ptr(new ImageLoadRunnablePrivate(this))
{
    d_ptr->viewPtr = view;
    d_ptr->fileUrl = fileUrl;
    d_ptr->taskCount = taskCount;

    setAutoDelete(true);
}

ImageLoadRunnable::~ImageLoadRunnable() {}

void ImageLoadRunnable::run()
{
    const QFileInfo file(d_ptr->fileUrl);
    const QFileInfoList list = file.absoluteDir().entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo &info : qAsConst(list)) {
        QImage image(info.absoluteFilePath());
        if (image.isNull()) {
            continue;
        }
        if (image.width() > WIDTH || image.height() > WIDTH) {
            image = image.scaled(WIDTH, WIDTH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        if (d_ptr->viewPtr.isNull()) {
            return;
        }
        if (!d_ptr->viewPtr->setImage(info, image, d_ptr->taskCount)) {
            return;
        }
    }
}

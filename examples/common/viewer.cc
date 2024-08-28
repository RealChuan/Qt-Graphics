#include "viewer.hpp"
#include "imagelistmodel.h"

#include <QDir>
#include <QPointer>
#include <QThreadPool>

Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
{}

Viewer::~Viewer()
{
    ImageLoadRunnable::terminateAll();
}

class ImageLoadRunnable::ImageLoadRunnablePrivate
{
public:
    explicit ImageLoadRunnablePrivate(ImageLoadRunnable *q)
        : q_ptr(q)
    {}

    ImageLoadRunnable *q_ptr;

    QPointer<Viewer> viewPtr;
    QString fileUrl;
    qint64 taskCount;

    static std::atomic_bool running;
};

std::atomic_bool ImageLoadRunnable::ImageLoadRunnablePrivate::running = true;

ImageLoadRunnable::ImageLoadRunnable(const QString &fileUrl, Viewer *view, qint64 taskCount)
    : d_ptr(new ImageLoadRunnablePrivate(this))
{
    d_ptr->viewPtr = view;
    d_ptr->fileUrl = fileUrl;
    d_ptr->taskCount = taskCount;

    setAutoDelete(true);
}

ImageLoadRunnable::~ImageLoadRunnable() = default;

void ImageLoadRunnable::terminateAll()
{
    ImageLoadRunnablePrivate::running.store(false);
    auto *instance = QThreadPool::globalInstance();
    instance->clear();
    instance->waitForDone();
}

void ImageLoadRunnable::run()
{
    const QFileInfo file(d_ptr->fileUrl);
    const QFileInfoList list = file.absoluteDir().entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo &info : std::as_const(list)) {
        QImage image(info.absoluteFilePath());
        if (image.isNull()) {
            continue;
        }
        if (image.width() > WIDTH || image.height() > WIDTH) {
            image = image.scaled(WIDTH, WIDTH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        if (d_ptr->viewPtr.isNull() || !d_ptr->running.load()
            || !d_ptr->viewPtr->setImage(info, image, d_ptr->taskCount)) {
            return;
        }
    }
}

#include "recordgifthread.hpp"
#include "recordwidget.hpp"

#include <3rdparty/gif/egif/GifEncoder.h>
#include <3rdparty/gif/gif-h/gif.h>
#include <utils/utils.h>

#include <QPixmap>

QString tempPath(const QString &name)
{
    auto path = QDir::tempPath();
    if (!path.endsWith("/")) {
        path += "/";
    }
    path += name + ".gif";
    return path;
}

void copy(const QString &srcPath, const QString &dstPath)
{
    if (QFile::exists(dstPath)) {
        QFile::remove(dstPath);
    }
    QFile::copy(srcPath, dstPath);
}

struct RecordGifThread::RecordGifThreadPrivate
{
    QPointer<RecordWidget> recordWidgetPtr;
    int interval = -1; // ms
    QSize size;
    QString savePath;
    QMutex mutex;
    QWaitCondition waitCondition;
    volatile bool runing = false;
    volatile bool capture = false;
};

RecordGifThread::RecordGifThread(QObject *parent)
    : QThread{parent}
    , d_ptr(new RecordGifThreadPrivate)
{}

RecordGifThread::~RecordGifThread()
{
    stop();
}

void RecordGifThread::startCapture(RecordWidget *recordWidget, int interval)
{
    Q_ASSERT(interval > 0);
    d_ptr->recordWidgetPtr = recordWidget;
    d_ptr->interval = interval;
    d_ptr->size = recordWidget->recordRect().size();
    d_ptr->runing = true;
    d_ptr->capture = true;
    start();
}

void RecordGifThread::stopCapture()
{
    d_ptr->capture = false;
}

void RecordGifThread::stop(const QString &savePath)
{
    d_ptr->savePath = savePath;
    d_ptr->runing = false;
    d_ptr->capture = false;
    if (isRunning()) {
        d_ptr->waitCondition.wakeAll();
        quit();
    }
    while (isRunning()) {
        qApp->processEvents();
    }
}

void RecordGifThread::run()
{
    auto f = qScopeGuard([] { qDebug() << "finished"; });
    /// [1]
    QScopedPointer<GifEncoder> gifEncoderPtr(createGifEncoder());
    QScopedPointer<GifWriter> gifWriterPtr(createGifWriter());

    QElapsedTimer timer;
    timer.start();
    while (d_ptr->runing) {
        auto delay = d_ptr->interval - timer.elapsed();
        if (delay > 0) { // use QTimer better
            QMutexLocker locker(&d_ptr->mutex);
            d_ptr->waitCondition.wait(&d_ptr->mutex, d_ptr->interval);
        }
        timer.restart();
        if (!d_ptr->capture) {
            continue;
        }
        auto pixmap = Utils::grabFullWindow();
        if (d_ptr->recordWidgetPtr.isNull()) {
            return;
        }
        pixmap = pixmap.copy(d_ptr->recordWidgetPtr->recordRect());
        auto image = pixmap.toImage();
        ///[2]
        push(gifEncoderPtr.data(), image);
        push(gifWriterPtr.data(), image);
    }
    ///[3]
    finish(gifEncoderPtr.data());
    finish(gifWriterPtr.data());

    if (d_ptr->savePath.isEmpty()) {
        return;
    }

    ///[4]
    moveFile1();
    moveFile2();
}

GifEncoder *RecordGifThread::createGifEncoder()
{
    auto savePath = tempPath("egif");

    int w = d_ptr->size.width();
    int h = d_ptr->size.height();
    int quality = 10;
    bool useGlobalColorMap = true;
    int loop = 0;
    int preAllocSize = useGlobalColorMap ? w * h * 3 * 3 : w * h * 3;
    std::unique_ptr<GifEncoder> gifEncoderPtr(new GifEncoder);
    if (!gifEncoderPtr
             ->open(savePath.toStdString(), w, h, quality, useGlobalColorMap, loop, preAllocSize)) {
        return nullptr;
    }
    return gifEncoderPtr.release();
}

void RecordGifThread::push(GifEncoder *encoder, const QImage &image)
{
    auto tmp = image;
    auto pixelFormat = GifEncoder::PixelFormat::PIXEL_FORMAT_UNKNOWN;
    switch (image.format()) {
    case QImage::Format_RGBA8888: pixelFormat = GifEncoder::PixelFormat::PIXEL_FORMAT_RGBA; break;
    case QImage::Format_BGR888: pixelFormat = GifEncoder::PixelFormat::PIXEL_FORMAT_BGR; break;
    case QImage::Format_RGB888:
    default:
        tmp = image.convertedTo(QImage::Format_RGB888);
        pixelFormat = GifEncoder::PixelFormat::PIXEL_FORMAT_RGB;
        break;
    }
    encoder->push(pixelFormat,
                  tmp.constBits(),
                  d_ptr->size.width(),
                  d_ptr->size.height(),
                  d_ptr->interval * 1.0 / 1000);
}

void RecordGifThread::finish(GifEncoder *encoder)
{
    qInfo() << encoder->close();
}

void RecordGifThread::moveFile1()
{
    auto savePath = d_ptr->savePath;
    savePath = savePath.replace(".gif", "_egif.gif");
    copy(tempPath("egif"), savePath);
}

GifWriter *RecordGifThread::createGifWriter()
{
    auto savePath = tempPath("gif-h");

    auto gifWriter = new GifWriter;
    GifBegin(gifWriter,
             savePath.toStdString().c_str(),
             d_ptr->size.width(),
             d_ptr->size.height(),
             d_ptr->interval * 1.0 / 10);
    return gifWriter;
}

void RecordGifThread::push(GifWriter *gifWriter, const QImage &image)
{
    auto tmp = image;
    switch (image.format()) {
    case QImage::Format_RGBA8888: break;
    default: tmp = image.convertedTo(QImage::Format_RGBA8888); break;
    }
    GifWriteFrame(gifWriter,
                  tmp.constBits(),
                  d_ptr->size.width(),
                  d_ptr->size.height(),
                  d_ptr->interval * 1.0 / 10);
}

void RecordGifThread::finish(GifWriter *gifWriter)
{
    GifEnd(gifWriter);
}

void RecordGifThread::moveFile2()
{
    auto savePath = d_ptr->savePath;
    savePath = savePath.replace(".gif", "_gif-h.gif");
    copy(tempPath("gif-h"), savePath);
}

void RecordGifThread::encode1(const QVector<QImage> &images)
{
    auto savePath = d_ptr->savePath;
    savePath = savePath.replace(".gif", "_egif.gif");

    int w = d_ptr->size.width();
    int h = d_ptr->size.height();
    int quality = 10;
    bool useGlobalColorMap = true;
    int loop = 0;
    int preAllocSize = useGlobalColorMap ? w * h * 3 * 3 : w * h * 3;
    QScopedPointer<GifEncoder> gifEncoderPtr(new GifEncoder);
    if (!gifEncoderPtr
             ->open(savePath.toStdString(), w, h, quality, useGlobalColorMap, loop, preAllocSize)) {
        return;
    }
    auto pixelFormat = GifEncoder::PixelFormat::PIXEL_FORMAT_UNKNOWN;
    for (auto image : qAsConst(images)) {
        switch (image.format()) {
        case QImage::Format_RGBA8888:
            pixelFormat = GifEncoder::PixelFormat::PIXEL_FORMAT_RGBA;
            break;
        case QImage::Format_BGR888: pixelFormat = GifEncoder::PixelFormat::PIXEL_FORMAT_BGR; break;
        case QImage::Format_RGB888:
        default:
            image = image.convertedTo(QImage::Format_RGB888);
            pixelFormat = GifEncoder::PixelFormat::PIXEL_FORMAT_RGB;
            break;
        }
        gifEncoderPtr->push(pixelFormat, image.constBits(), w, h, d_ptr->interval * 1.0 / 1000);
    }
    qInfo() << gifEncoderPtr->close();
}

void RecordGifThread::encode2(const QVector<QImage> &images)
{
    auto savePath = d_ptr->savePath;
    savePath = savePath.replace(".gif", "_gif-h.gif");

    int w = d_ptr->size.width();
    int h = d_ptr->size.height();
    auto delay = d_ptr->interval * 1.0 / 10;

    QScopedPointer<GifWriter> gifWriterPtr(new GifWriter);
    GifBegin(gifWriterPtr.data(), savePath.toStdString().c_str(), w, h, delay);
    for (auto image : qAsConst(images)) {
        switch (image.format()) {
        case QImage::Format_RGBA8888: break;
        default: image = image.convertedTo(QImage::Format_RGBA8888); break;
        }
        GifWriteFrame(gifWriterPtr.data(), image.constBits(), w, h, delay);
    }
    GifEnd(gifWriterPtr.data());
}

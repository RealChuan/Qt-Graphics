#pragma once

#include <utils/utils.hpp>

class GifEncoder;
struct GifWriter;

class RecordWidget;

class RecordGifThread : public QThread
{
    Q_OBJECT
public:
    explicit RecordGifThread(QObject *parent = nullptr);
    ~RecordGifThread() override;

    void startCapture(RecordWidget *recordWidget, int interval);
    void stopCapture();
    void startStop(const QString &savePath);
    void stop(const QString &savePath = {});

protected:
    void run() override;

private:
    /// [1]
    auto createGifEncoder() -> GifEncoder *;
    void push(GifEncoder *encoder, const QImage &image);
    void finish(GifEncoder *encoder);
    void moveFile1();
    ///[2]
    auto createGifWriter() -> GifWriter *;
    void push(GifWriter *gifWriter, const QImage &image);
    void finish(GifWriter *gifWriter);
    void moveFile2();

    // 太占内存
    void encode1(const Utils::Images &images);
    void encode2(const Utils::Images &images);

    void encode1();
    void encode2();

    class RecordGifThreadPrivate;
    QScopedPointer<RecordGifThreadPrivate> d_ptr;
};

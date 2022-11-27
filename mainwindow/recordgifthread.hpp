#ifndef RECORDGIFTHREAD_HPP
#define RECORDGIFTHREAD_HPP

#include <QThread>

class GifEncoder;
struct GifWriter;

class RecordWidget;

class RecordGifThread : public QThread
{
    Q_OBJECT
public:
    explicit RecordGifThread(QObject *parent = nullptr);
    ~RecordGifThread();

    void startCapture(RecordWidget *recordWidget, int interval);
    void stopCapture();
    void stop(const QString &savePath = {});

protected:
    void run() override;

private:
    /// [1]
    GifEncoder *createGifEncoder();
    void push(GifEncoder *encoder, const QImage &image);
    void finish(GifEncoder *encoder);
    void moveFile1();
    ///[2]
    GifWriter *createGifWriter();
    void push(GifWriter *gifWriter, const QImage &image);
    void finish(GifWriter *gifWriter);
    void moveFile2();

    // 太占内存
    void encode1(const QVector<QImage> &images);
    void encode2(const QVector<QImage> &images);

    void encode1();
    void encode2();

    struct RecordGifThreadPrivate;
    QScopedPointer<RecordGifThreadPrivate> d_ptr;
};

#endif // RECORDGIFTHREAD_HPP

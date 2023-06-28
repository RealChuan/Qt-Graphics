#pragma once

#include <QFileInfo>
#include <QRunnable>
#include <QWidget>

class Viewer : public QWidget
{
public:
    explicit Viewer(QWidget *parent = nullptr);
    ~Viewer() override;

    virtual auto setImage(const QFileInfo &info, const QImage &image, const qint64 taskCount) -> bool = 0;
};

class ImageLoadRunnable : public QRunnable
{
public:
    ImageLoadRunnable(const QString &fileUrl, Viewer *view, qint64 taskCount);
    ~ImageLoadRunnable() override;

protected:
    void run() override;

private:
    class ImageLoadRunnablePrivate;
    QScopedPointer<ImageLoadRunnablePrivate> d_ptr;
};

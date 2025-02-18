#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <examples/common/viewer.hpp>

class ImageViewer : public Viewer
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer() override;

private slots:
    void onOpenImage();
    void onMaskImage();
    void onRoundImage();

    void onChangedImage(int);

    void onFormatChecked(bool);
    void onFormatChanged(const QString &);

private:
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class ImageViewerPrivate;
    QScopedPointer<ImageViewerPrivate> d_ptr;
};

#endif // IMAGEVIEWER_H

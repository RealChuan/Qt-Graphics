#pragma once

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

    void onChangedImage(int index);

    void onFormatChecked(bool checked);
    void onFormatChanged(const QString &format);

private:
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class ImageViewerPrivate;
    QScopedPointer<ImageViewerPrivate> d_ptr;
};

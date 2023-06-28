#ifndef IMAGECAPTUREVIEW_HPP
#define IMAGECAPTUREVIEW_HPP

#include "listiitemview.hpp"

class ImageCaptureView : public ListItemView
{
    Q_OBJECT
public:
    explicit ImageCaptureView(QWidget *parent = nullptr);
    ~ImageCaptureView() override;

    void setImageAfter() override;

private:
    class ImageCaptureViewPrivate;
    QScopedPointer<ImageCaptureViewPrivate> d_ptr;
};

#endif // IMAGECAPTUREVIEW_HPP

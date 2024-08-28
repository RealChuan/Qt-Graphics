#include "imagecaptureview.hpp"

#include <graphics/graphicsrectitem.h>

class ImageCaptureView::ImageCaptureViewPrivate
{
public:
    explicit ImageCaptureViewPrivate(ImageCaptureView *q)
        : q_ptr(q)
        , rectPtr(new Graphics::GraphicsRectItem)
    {}
    ~ImageCaptureViewPrivate() {}

    ImageCaptureView *q_ptr;
    QScopedPointer<Graphics::GraphicsRectItem> rectPtr;
};

ImageCaptureView::ImageCaptureView(QWidget *parent)
    : ListItemView(parent)
    , d_ptr(new ImageCaptureViewPrivate(this))
{}

ImageCaptureView::~ImageCaptureView() {}

void ImageCaptureView::setImageAfter()
{
    auto rect = sceneRect();
    rect.adjust(2, 2, -2, -2);
    d_ptr->rectPtr->setRect(rect);
    scene()->addItem(d_ptr->rectPtr.data());
}

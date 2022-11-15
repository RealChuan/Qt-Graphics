#include "imagecaptureview.hpp"

#include <graphics/graphicsrectitem.h>

class ImageCaptureView::ImageCaptureViewPrivate
{
public:
    ImageCaptureViewPrivate(QWidget *parent)
        : owner(parent)
        , rectPtr(new Graphics::GraphicsRectItem)
    {}
    ~ImageCaptureViewPrivate() {}

    QWidget *owner;
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

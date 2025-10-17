#include "graphicsrectitem.h"

namespace Graphics {

GraphicsRectItem::GraphicsRectItem(QGraphicsItem *parent)
    : GraphicsRoundedRectItem(parent)
{
    m_roundedRect = m_tempRoundedRect = RoundedRect(QRectF());
    buildConnect();
}

GraphicsRectItem::GraphicsRectItem(const QRectF &rect, QGraphicsItem *parent)
    : GraphicsRoundedRectItem(parent)
{
    buildConnect();
    setRect(rect);
}

GraphicsRectItem::~GraphicsRectItem() {}

auto GraphicsRectItem::setRect(const QRectF &rect) -> bool
{
    RoundedRect roundedRect(rect);
    return setRoundedRect(roundedRect);
}

QRectF GraphicsRectItem::rect() const
{
    return roundedRect().rect;
}

int GraphicsRectItem::type() const
{
    return RECT;
}

void GraphicsRectItem::onRoundedRectChanged(const RoundedRect &roundedRect)
{
    emit rectChanged(roundedRect.rect);
}

void GraphicsRectItem::buildConnect()
{
    connect(this,
            &GraphicsRectItem::roundedRectChanged,
            this,
            &GraphicsRectItem::onRoundedRectChanged);
}

} // namespace Graphics

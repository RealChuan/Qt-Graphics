#include "drawwidget.h"

#include <graphics/basicgraphicsitem.h>
#include <graphics/graphicsarcitem.h>
#include <graphics/graphicscircleitem.h>
#include <graphics/graphicslineitem.h>
#include <graphics/graphicspolygonitem.h>
#include <graphics/graphicsrectitem.h>
#include <graphics/graphicsringitem.h>
#include <graphics/graphicsrotatedrectitem.h>
#include <graphics/imageview.h>

#include <QDebug>
#include <QtWidgets>

using namespace Graphics;

class DrawWidget::DrawWidgetPrivate
{
public:
    DrawWidgetPrivate(QWidget *parent)
        : owner(parent)
    {
        imageView = new ImageView(owner);
        imageView->setContextMenuPolicy(Qt::NoContextMenu);
        shapeWidget = new QListWidget(owner);
        shapeWidget->setFixedWidth(200);

        QPixmap pixmap(4096, 2160);
        pixmap.fill(Qt::white);
        imageView->setPixmap(pixmap);
        imageView->resetToOriginalSize();
    }
    QWidget *owner;
    ImageView *imageView;
    QListWidget *shapeWidget;
    QVector<BasicGraphicsItem *> graphicsItems;
};

DrawWidget::DrawWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new DrawWidgetPrivate(this))
{
    setupUI();
}

DrawWidget::~DrawWidget()
{
    if (d_ptr->graphicsItems.isEmpty()) {
        return;
    }
    qDeleteAll(d_ptr->graphicsItems);
    d_ptr->graphicsItems.clear();
}

void DrawWidget::onAddShape(QListWidgetItem *item)
{
    if (!d_ptr->graphicsItems.isEmpty() && !d_ptr->graphicsItems.last()->isValid()) {
        delete d_ptr->graphicsItems.takeLast();
    }

    BasicGraphicsItem *shape = nullptr;
    int type = d_ptr->shapeWidget->row(item) + 1;
    switch (type) {
    case BasicGraphicsItem::LINE: shape = new GraphicsLineItem; break;
    case BasicGraphicsItem::RECT: shape = new GraphicsRectItem; break;
    case BasicGraphicsItem::CIRCLE: shape = new GraphicsCircleItem; break;
    case BasicGraphicsItem::POLYGON: shape = new GraphicsPolygonItem; break;
    case BasicGraphicsItem::RING: shape = new GraphicsRingItem; break;
    case BasicGraphicsItem::ARC: shape = new GraphicsArcItem; break;
    case BasicGraphicsItem::ROTATEDRECT: shape = new GraphicsRotatedRectItem; break;
    default: break;
    }
    if (!shape) {
        return;
    }

    d_ptr->graphicsItems.append(shape);
    d_ptr->imageView->scene()->addItem(shape);
    connect(shape,
            &BasicGraphicsItem::deleteMyself,
            this,
            &DrawWidget::onRemoveShape,
            Qt::UniqueConnection);
}

void DrawWidget::onRemoveShape()
{
    BasicGraphicsItem *item = qobject_cast<BasicGraphicsItem *>(sender());
    if (!item) {
        return;
    }
    d_ptr->graphicsItems.removeOne(item);
    item->deleteLater();
}

void DrawWidget::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d_ptr->shapeWidget);
    layout->addWidget(d_ptr->imageView);

    QMetaEnum shapeEnum = QMetaEnum::fromType<BasicGraphicsItem::Shape>();
    for (int i = 0; i < shapeEnum.keyCount(); i++) {
        auto item = new QListWidgetItem(shapeEnum.key(i), d_ptr->shapeWidget);
        item->setSizeHint(QSize(1, 30));
        item->setTextAlignment(Qt::AlignCenter);
        d_ptr->shapeWidget->addItem(item);
    }
    connect(d_ptr->shapeWidget, &QListWidget::itemClicked, this, &DrawWidget::onAddShape);
}

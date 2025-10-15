#include "drawwidget.h"
#include "drawscene.hpp"

#include <graphics/graphicsarcitem.h>
#include <graphics/graphicsbasicitem.h>
#include <graphics/graphicscircleitem.h>
#include <graphics/graphicslineitem.h>
#include <graphics/graphicspolygonitem.h>
#include <graphics/graphicsrectitem.h>
#include <graphics/graphicsringitem.h>
#include <graphics/graphicsrotatedrectitem.h>
#include <graphics/graphicsroundedrectitem.hpp>
#include <graphics/graphicstextitem.hpp>
#include <graphics/graphicsview.hpp>
#include <utils/validator.hpp>

#include <QDebug>
#include <QtWidgets>

using namespace Graphics;

class DrawWidget::DrawWidgetPrivate
{
public:
    explicit DrawWidgetPrivate(DrawWidget *parent)
        : q_ptr(parent)
    {
        drawScene = new DrawScene(q_ptr);
        imageView = new GraphicsView(drawScene, q_ptr);
        imageView->setContextMenuPolicy(Qt::NoContextMenu);
        shapeWidget = new QListWidget(q_ptr);
        shapeWidget->setFixedWidth(200);

        newButton = new QToolButton(q_ptr);
        newButton->setText(tr("New", "DrawWidget"));

        fontCombo = new QFontComboBox(q_ptr);
        fontSizeCombo = new QComboBox(q_ptr);
        fontSizeCombo->setEditable(true);
        for (int i = 8; i < 100; i = i + 2) {
            fontSizeCombo->addItem(QString().setNum(i));
        }
        fontSizeCombo->setValidator(new Utils::IntValidator(2, 64, q_ptr));

        fontColorToolButton = new QToolButton(q_ptr);
        fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
        fontColorToolButton->setAutoFillBackground(true);
        boldAction = new QAction(tr("Bold", "DrawWidget"), q_ptr);
        boldAction->setCheckable(true);
        boldAction->setShortcut(tr("Ctrl+B"));
        italicAction = new QAction(tr("Italic", "DrawWidget"), q_ptr);
        italicAction->setCheckable(true);
        italicAction->setShortcut(tr("Ctrl+I"));
        underlineAction = new QAction(tr("Underline", "DrawWidget"), q_ptr);
        underlineAction->setCheckable(true);
        underlineAction->setShortcut(tr("Ctrl+U"));
    }

    DrawWidget *q_ptr;

    DrawScene *drawScene;
    GraphicsView *imageView;
    QListWidget *shapeWidget;
    GraphicsItemList graphicsItemList;

    QToolButton *newButton;
    QComboBox *fontSizeCombo;
    QFontComboBox *fontCombo;
    QAction *boldAction;
    QAction *underlineAction;
    QAction *italicAction;
    QAction *textAction;
    QToolButton *fontColorToolButton;
};

DrawWidget::DrawWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new DrawWidgetPrivate(this))
{
    setupUI();
    buildConnect();
}

DrawWidget::~DrawWidget()
{
    clearAll();
}

void DrawWidget::onAddShape(QListWidgetItem *item)
{
    if (d_ptr->imageView->pixmap().isNull()) {
        clearAll();
        QMessageBox::warning(this, tr("WARNING"), tr("Please create a new canvas"));
        return;
    }

    if (!d_ptr->graphicsItemList.isEmpty() && !d_ptr->graphicsItemList.last()->isValid()) {
        delete d_ptr->graphicsItemList.takeLast();
    }

    GraphicsBasicItem *shape = nullptr;
    int type = d_ptr->shapeWidget->row(item) + 1;
    switch (type) {
    case GraphicsBasicItem::LINE: shape = new GraphicsLineItem; break;
    case GraphicsBasicItem::RECT: shape = new GraphicsRectItem; break;
    case GraphicsBasicItem::ROUNDEDRECT: shape = new GraphicsRoundedRectItem; break;
    case GraphicsBasicItem::ROTATEDRECT: shape = new GraphicsRotatedRectItem; break;
    case GraphicsBasicItem::CIRCLE: shape = new GraphicsCircleItem; break;
    case GraphicsBasicItem::POLYGON: shape = new GraphicsPolygonItem; break;
    case GraphicsBasicItem::RING: shape = new GraphicsRingItem; break;
    case GraphicsBasicItem::ARC: shape = new GraphicsArcItem; break;
    default: break;
    }
    if (shape) {
        d_ptr->graphicsItemList.append(shape);
        d_ptr->drawScene->addItem(shape);
        d_ptr->drawScene->setDrawText(false);
    } else {
        d_ptr->drawScene->setDrawText(true);
    }
}

void DrawWidget::onDeleteItem()
{
    auto selectedItems = d_ptr->drawScene->selectedItems();
    for (auto item : selectedItems) {
        auto basicGraphicsItem = dynamic_cast<GraphicsBasicItem *>(item);
        if (basicGraphicsItem) {
            d_ptr->graphicsItemList.removeOne(basicGraphicsItem);
            basicGraphicsItem->deleteLater();
        } else {
            auto textItem = qgraphicsitem_cast<GraphicsTextItem *>(item);
            if (textItem) {
                delete textItem;
            }
        }
    }
}

void DrawWidget::onSave()
{
    if (d_ptr->imageView->pixmap().isNull()) {
        return;
    }
    auto path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                    .value(0, QDir::homePath());
    const auto time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    path = path + "/" + time + ".jpg";
    const QString filename = QFileDialog::getSaveFileName(nullptr,
                                                          tr("Save Image"),
                                                          path,
                                                          tr("Images (*.png *.xpm *.jpg)"));
    if (filename.isEmpty()) {
        return;
    }
    auto size = d_ptr->drawScene->sceneRect().size();
    QPixmap pixmap(size.width(), size.height());
    QPainter painter(&pixmap);
    d_ptr->drawScene->render(&painter);
    qInfo() << pixmap.save(filename);
}

void DrawWidget::handleFontChange()
{
    auto font = d_ptr->fontCombo->currentFont();
    font.setPointSize(d_ptr->fontSizeCombo->currentText().toInt());
    font.setWeight(d_ptr->boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(d_ptr->italicAction->isChecked());
    font.setUnderline(d_ptr->underlineAction->isChecked());

    d_ptr->drawScene->setFont(font);

    auto selectedItems = d_ptr->drawScene->selectedItems();
    for (auto item : selectedItems) {
        auto textItem = qgraphicsitem_cast<GraphicsTextItem *>(item);
        if (textItem) {
            textItem->setFont(font);
        }
    }
}

void DrawWidget::textButtonTriggered()
{
    d_ptr->drawScene->setTextColor(qvariant_cast<QColor>(d_ptr->textAction->data()));
}

void DrawWidget::textColorChanged()
{
    d_ptr->textAction = qobject_cast<QAction *>(sender());
    d_ptr->fontColorToolButton->setIcon(
        createColorToolButtonIcon(qvariant_cast<QColor>(d_ptr->textAction->data())));
    textButtonTriggered();
}

void DrawWidget::itemSelected(QGraphicsItem *item)
{
    auto textItem = qgraphicsitem_cast<GraphicsTextItem *>(item);
    if (!textItem) {
        return;
    }

    QFont font = textItem->font();
    d_ptr->fontCombo->setCurrentFont(font);
    d_ptr->fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    d_ptr->boldAction->setChecked(font.weight() == QFont::Bold);
    d_ptr->italicAction->setChecked(font.italic());
    d_ptr->underlineAction->setChecked(font.underline());
}

void DrawWidget::setupUI()
{
    QMetaEnum shapeEnum = QMetaEnum::fromType<GraphicsBasicItem::Shape>();
    for (int i = 0; i < shapeEnum.keyCount(); i++) {
        auto item = new QListWidgetItem(shapeEnum.key(i), d_ptr->shapeWidget);
        item->setSizeHint(QSize(1, 30));
        item->setTextAlignment(Qt::AlignCenter);
        d_ptr->shapeWidget->addItem(item);
    }
    auto item = new QListWidgetItem(tr("Text"), d_ptr->shapeWidget);
    item->setSizeHint(QSize(1, 30));
    item->setTextAlignment(Qt::AlignCenter);
    d_ptr->shapeWidget->addItem(item);

    auto bodyLayout = new QHBoxLayout;
    bodyLayout->setContentsMargins({});
    bodyLayout->setSpacing(0);
    bodyLayout->addWidget(d_ptr->shapeWidget);
    bodyLayout->addWidget(d_ptr->imageView);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->setSpacing(0);
    layout->addWidget(cerateToolBar());
    layout->addLayout(bodyLayout);
}

void DrawWidget::buildConnect()
{
    connect(d_ptr->shapeWidget, &QListWidget::itemClicked, this, &DrawWidget::onAddShape);
    connect(d_ptr->drawScene, &DrawScene::deleteItem, this, &DrawWidget::onDeleteItem);
    connect(d_ptr->drawScene, &DrawScene::itemSelected, this, &DrawWidget::itemSelected);
    new QShortcut(QKeySequence::SelectAll, this, this, [this] {
        auto items = d_ptr->drawScene->items();
        for (auto item : std::as_const(items)) {
            item->setSelected(true);
        }
    });
    initNewButton();
}

void DrawWidget::initNewButton()
{
    auto *menu = new QMenu(this);
    menu->addAction(tr("Create a New Canvas 4096 x 2160"), this, [this] {
        QPixmap pixmap(4096, 2160);
        pixmap.fill(Qt::white);
        d_ptr->imageView->setPixmap(pixmap);
        d_ptr->imageView->resetToOriginalSize();
    });
    menu->addAction(tr("Load an image"), this, [this] {
        QString imageFilters(tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.svg *.tiff *.webp *.icns "
                                "*.bitmap *.graymap *.pixmap *.tga *.xbitmap *.xpixmap)"));
        //qDebug() << imageFilters;
        const QString path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                                 .value(0, QDir::homePath());
        const QString filename = QFileDialog::getOpenFileName(this,
                                                              tr("Open Image"),
                                                              path,
                                                              imageFilters);
        if (filename.isEmpty()) {
            return;
        }
        QPixmap pixmap(filename);
        if (pixmap.isNull()) {
            QMessageBox::warning(this,
                                 tr("WARNING"),
                                 tr("Picture failed to open, Url: %1!").arg(filename));
            return;
        }
        d_ptr->imageView->setPixmap(pixmap);
        d_ptr->imageView->resetToOriginalSize();
    });
    d_ptr->newButton->setMenu(menu);
    d_ptr->newButton->setPopupMode(QToolButton::InstantPopup);
}

void DrawWidget::clearAll()
{
    if (d_ptr->graphicsItemList.isEmpty()) {
        return;
    }
    qDeleteAll(d_ptr->graphicsItemList);
    d_ptr->graphicsItemList.clear();
}

QToolBar *DrawWidget::cerateToolBar()
{
    connect(d_ptr->boldAction, &QAction::triggered, this, &DrawWidget::handleFontChange);
    connect(d_ptr->italicAction, &QAction::triggered, this, &DrawWidget::handleFontChange);
    connect(d_ptr->underlineAction, &QAction::triggered, this, &DrawWidget::handleFontChange);

    connect(d_ptr->fontCombo,
            &QFontComboBox::currentFontChanged,
            this,
            &DrawWidget::handleFontChange);
    connect(d_ptr->fontSizeCombo,
            &QComboBox::currentTextChanged,
            this,
            &DrawWidget::handleFontChange);

    d_ptr->fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
    d_ptr->textAction = d_ptr->fontColorToolButton->menu()->defaultAction();
    d_ptr->fontColorToolButton->setIcon(createColorToolButtonIcon(Qt::black));
    connect(d_ptr->fontColorToolButton,
            &QAbstractButton::clicked,
            this,
            &DrawWidget::textButtonTriggered);

    auto textToolBar = new QToolBar(tr("Font"), this);
    textToolBar->addWidget(d_ptr->newButton);
    textToolBar->addAction(tr("Save As"), this, &DrawWidget::onSave);
    textToolBar->addWidget(d_ptr->fontCombo);
    textToolBar->addWidget(d_ptr->fontSizeCombo);
    textToolBar->addAction(d_ptr->boldAction);
    textToolBar->addAction(d_ptr->italicAction);
    textToolBar->addAction(d_ptr->underlineAction);
    textToolBar->addWidget(d_ptr->fontColorToolButton);

    return textToolBar;
}

QMenu *DrawWidget::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue") << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor) {
            colorMenu->setDefaultAction(action);
        }
    }
    return colorMenu;
}

QIcon DrawWidget::createColorToolButtonIcon(QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    //QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    QFont font;
    font.setPixelSize(40);
    painter.setFont(font);
    painter.drawText(target, Qt::AlignCenter, "A");

    return QIcon(pixmap);
}

QIcon DrawWidget::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}

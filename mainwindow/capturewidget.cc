#include "capturewidget.hpp"
#include "selectionwidget.hpp"

#include <utils/utils.h>

#include <QtWidgets>

class CaptureWidget::CaptureWidgetPrivate
{
public:
    CaptureWidgetPrivate(QWidget *parent)
        : owner(parent)
    {
        desktopGeometry = Utils::desktopGeometry();
        origScreenshot = Utils::grabFullWindow();
        screenshot = origScreenshot;
        selectionWidget = new SelectionWidget(uiColor, owner);
        selectionWidget->hide();
        menu = new QMenu(owner);
        saveAsAction = new QAction(tr("Save As...", "CaptureWidget"), owner);
        menu->addAction(saveAsAction);
    }

    QWidget *owner;

    QPixmap origScreenshot;
    QPixmap screenshot;
    QRect desktopGeometry;

    SelectionWidget *selectionWidget;
    QMenu *menu;
    QAction *saveAsAction;

    QColor uiColor = QColor(116, 0, 150);
};

CaptureWidget::CaptureWidget(QWidget *parent)
    : QWidget{parent}
    , d_ptr(new CaptureWidgetPrivate(this))
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);

    move(d_ptr->desktopGeometry.topLeft());
    resize(d_ptr->screenshot.size());

    buildConnect();
}

CaptureWidget::~CaptureWidget() {}

void CaptureWidget::onSelectAll()
{
    d_ptr->selectionWidget->show();
    d_ptr->selectionWidget->setGeometry(rect());
}

void CaptureWidget::onSave()
{
    hide();
    auto path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                    .value(0, QDir::homePath());
    const auto time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    path = path + "/" + time + ".png";
    const QString filename = QFileDialog::getSaveFileName(nullptr,
                                                          tr("Save Image"),
                                                          path,
                                                          tr("Images (*.png *.xpm *.jpg)"));
    if (filename.isEmpty()) {
        show();
        return;
    }
    auto rect = d_ptr->selectionWidget->geometry();
    auto pixmap = d_ptr->screenshot.copy(rect);
    pixmap = pixmap.copy(rect);
    qInfo() << rect << pixmap.save(filename);
    close();
}

void CaptureWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform
                           | QPainter::TextAntialiasing);
    painter.drawPixmap(rect(), d_ptr->screenshot);
    QRect r;
    if (d_ptr->selectionWidget->isVisible()) {
        r = d_ptr->selectionWidget->geometry().normalized();
    }
    drawText(&painter, r);
    QRegion grey(rect());
    grey = grey.subtracted(r);
    painter.setClipRegion(grey);
    painter.fillRect(rect(), QColor(160, 160, 160, 200));
}

void CaptureWidget::contextMenuEvent(QContextMenuEvent *event)
{
    d_ptr->saveAsAction->setVisible(d_ptr->selectionWidget->isVisible());
    d_ptr->menu->exec(event->globalPos());
}

void CaptureWidget::buildConnect()
{
    // fix 残留 ，但是吃性能
    //connect(d_ptr->selectionWidget, &SelectionWidget::geometryChanged, this, [this] { update(); });
    connect(d_ptr->saveAsAction, &QAction::triggered, this, &CaptureWidget::onSave);
    d_ptr->menu->addAction(tr("Select All"), this, &CaptureWidget::onSelectAll);
    d_ptr->menu->addAction(tr("Quit"), this, &CaptureWidget::close);

    new QShortcut(QKeySequence::Save, this, this, &CaptureWidget::onSave);
    new QShortcut(QKeySequence::SelectAll, this, this, &CaptureWidget::onSelectAll);
    new QShortcut(QKeySequence::Cancel, this, this, &CaptureWidget::close);
}

void CaptureWidget::drawText(QPainter *painter, const QRect &rect)
{
    if (!rect.isValid()) {
        return;
    }
    //const qreal scale = d_ptr->screenshot.devicePixelRatio();
    auto str = QString("%1x%2+%3+%4")
                   .arg(static_cast<int>(rect.width()))
                   .arg(static_cast<int>(rect.height()))
                   .arg(static_cast<int>(rect.left()))
                   .arg(static_cast<int>(rect.top()));
    auto fontSize = qMin(rect.width(), rect.height()) / 30;
    fontSize = fontSize > 14 ? fontSize : 14;
    QFont font;
    font.setPixelSize(fontSize);
    painter->setFont(font);
    painter->setPen(Qt::white);
    QRect xybox = painter->fontMetrics().boundingRect(str);
    xybox.adjust(0, 0, 10, 12);
    int x = rect.left() + (rect.width() - xybox.width()) / 2;
    int y = rect.top();
    QRect r(x, y, xybox.width(), xybox.height());
    painter->fillRect(r, d_ptr->uiColor);
    painter->drawText(r, Qt::AlignCenter, str);
}

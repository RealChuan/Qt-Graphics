#include "capturewidget.hpp"

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
        QPixmap pix(screenshot.size());
        pix.fill((QColor(160, 160, 160, 200)));
        QPainter painter(&screenshot);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        painter.drawPixmap(0, 0, pix);

        menu = new QMenu(owner);
    }

    QWidget *owner;

    QPixmap origScreenshot;
    QPixmap screenshot;
    QRect desktopGeometry;

    QMenu *menu;
};

CaptureWidget::CaptureWidget(QWidget *parent)
    : QWidget{parent}
    , d_ptr(new CaptureWidgetPrivate(this))
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    move(d_ptr->desktopGeometry.topLeft());
    resize(d_ptr->screenshot.size());

    buildConnect();
}

CaptureWidget::~CaptureWidget() {}

void CaptureWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.drawPixmap(rect(), d_ptr->screenshot);
}

void CaptureWidget::contextMenuEvent(QContextMenuEvent *event)
{
    d_ptr->menu->exec(event->globalPos());
}

void CaptureWidget::buildConnect()
{
    d_ptr->menu->addAction(tr("quit"), this, &CaptureWidget::close);
}

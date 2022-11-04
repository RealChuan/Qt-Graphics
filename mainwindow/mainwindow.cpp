#include "mainwindow.h"
#include "drawwidget.h"
#include "imageviewer.h"

#include <utils/utils.h>

#include <QtWidgets>

class MainWindow::MainWindowPrivate
{
public:
    MainWindowPrivate(QWidget *parent)
        : owner(parent)
    {
        drawWidget = new DrawWidget(owner);
        imageViewer = new ImageViewer(owner);
        stackedWidget = new QStackedWidget(owner);
        stackedWidget->addWidget(imageViewer);
        stackedWidget->addWidget(drawWidget);
    }
    QWidget *owner;
    DrawWidget *drawWidget;
    ImageViewer *imageViewer;
    QStackedWidget *stackedWidget;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d_ptr(new MainWindowPrivate(this))
{
    setupUI();
    resize(1000, 618);
    Utils::windowCenter(this);
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    // 图片读取限制大小
    qInfo() << tr("QImage memory allocation above this limit: %1MB.")
                   .arg(QImageReader::allocationLimit());
    //QImageReader::setAllocationLimit();
#endif
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    initMenuBar();
    setCentralWidget(d_ptr->stackedWidget);
}

void MainWindow::initMenuBar()
{
    auto menu = new QMenu(tr("Select Widget"), this);
    menu->addAction(tr("ImageViewer"), this, [this] {
        d_ptr->stackedWidget->setCurrentWidget(d_ptr->imageViewer);
    });
    menu->addAction(tr("DrawWidget"), this, [this] {
        d_ptr->stackedWidget->setCurrentWidget(d_ptr->drawWidget);
    });
    menuBar()->addMenu(menu);
}

#include "mainwindow.hpp"
#include "opencvwidget.hpp"

#include <utils/utils.h>

#include <QtWidgets>

class MainWindow::MainWindowPrivate
{
public:
    explicit MainWindowPrivate(MainWindow *q)
        : q_ptr(q)
    {}

    MainWindow *q_ptr;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d_ptr(new MainWindowPrivate(this))
{
    setCentralWidget(new OpenCVWidget(this));
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

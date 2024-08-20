#include "mainwindow.hpp"
#include "rhiviewer.hpp"

#include <utils/utils.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(new RhiViewer(this));

    resize(1000, 618);
    Utils::windowCenter(this);
}

MainWindow::~MainWindow() {}

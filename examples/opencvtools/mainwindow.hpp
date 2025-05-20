#pragma once

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    class MainWindowPrivate;
    QScopedPointer<MainWindowPrivate> d_ptr;
};

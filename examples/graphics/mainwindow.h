#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void setupUI();
    void initMenuBar();

    class MainWindowPrivate;
    QScopedPointer<MainWindowPrivate> d_ptr;
};
#endif // MAINWINDOW_H

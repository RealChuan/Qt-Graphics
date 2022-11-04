#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>

class QListWidgetItem;
class DrawWidget : public QWidget
{
public:
    explicit DrawWidget(QWidget *parent = nullptr);
    ~DrawWidget();

private slots:
    void onAddShape(QListWidgetItem*);
    void onRemoveShape();

private:
    void setupUI();

    class DrawWidgetPrivate;
    QScopedPointer<DrawWidgetPrivate> d_ptr;
};

#endif // DRAWWIDGET_H

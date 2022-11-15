#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>

class QToolBar;
class QListWidgetItem;
class QGraphicsItem;

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawWidget(QWidget *parent = nullptr);
    ~DrawWidget();

private slots:
    void onAddShape(QListWidgetItem *);
    void onDeleteItem();

    void handleFontChange();
    void textButtonTriggered();
    void textColorChanged();
    void itemSelected(QGraphicsItem *item);

private:
    void setupUI();
    void buildConnect();

    QToolBar *cerateToolBar();
    QMenu *createColorMenu(const char *slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(QColor color);
    QIcon createColorIcon(QColor color);

    class DrawWidgetPrivate;
    QScopedPointer<DrawWidgetPrivate> d_ptr;
};

#endif // DRAWWIDGET_H

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
    ~DrawWidget() override;

private slots:
    void onAddShape(QListWidgetItem *);
    void onDeleteItem();
    void onSave();

    void handleFontChange();
    void textButtonTriggered();
    void textColorChanged();
    void itemSelected(QGraphicsItem *item);

private:
    void setupUI();
    void buildConnect();
    void initNewButton();
    void clearAll();

    auto cerateToolBar() -> QToolBar *;
    auto createColorMenu(const char *slot, QColor defaultColor) -> QMenu *;
    auto createColorToolButtonIcon(QColor color) -> QIcon;
    auto createColorIcon(QColor color) -> QIcon;

    class DrawWidgetPrivate;
    QScopedPointer<DrawWidgetPrivate> d_ptr;
};

#endif // DRAWWIDGET_H

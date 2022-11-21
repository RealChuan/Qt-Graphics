#ifndef RECORDWIDGET_HPP
#define RECORDWIDGET_HPP

#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

class RecordWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RecordWidget(QWidget *parent = nullptr);
    ~RecordWidget();

private slots:
    void onChangeSize();
    void onResizeGifWidget();
    void onSaveGrab();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void buildConnect();

    class RecordWidgetPrivate;
    QScopedPointer<RecordWidgetPrivate> d_ptr;
};

#endif // RECORDWIDGET_HPP

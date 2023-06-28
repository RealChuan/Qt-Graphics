#ifndef CAPTUREWIDGET_HPP
#define CAPTUREWIDGET_HPP

#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

class CaptureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CaptureWidget(QWidget *parent = nullptr);
    ~CaptureWidget() override;

private:
    void onSelectAll();
    void onSave();

protected:
    auto eventFilter(QObject *obj, QEvent *event) -> bool override;
    void paintEvent(QPaintEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void buildConnect();
    void drawText(QPainter *painter, const QRect &rect);

    class CaptureWidgetPrivate;
    QScopedPointer<CaptureWidgetPrivate> d_ptr;
};

#endif // CAPTUREWIDGET_HPP

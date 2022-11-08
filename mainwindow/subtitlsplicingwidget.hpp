#ifndef SUBTITLSPLICINGWIDGET_HPP
#define SUBTITLSPLICINGWIDGET_HPP

#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

class SubtitlSplicingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SubtitlSplicingWidget(QWidget *parent = nullptr);
    ~SubtitlSplicingWidget();

    void setImage(const QImage &image);

private slots:
    void onOpenImage();
    void onGenerated();
    void onSave();
    void onUp(int index);
    void onDown(int index);

private:
    void setupUI();

    class SubtitlSplicingWidgetPrivate;
    QScopedPointer<SubtitlSplicingWidgetPrivate> d_ptr;
};

#endif // SUBTITLSPLICINGWIDGET_HPP

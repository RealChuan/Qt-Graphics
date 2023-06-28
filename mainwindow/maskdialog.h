#ifndef MASKDIALOG_H
#define MASKDIALOG_H

#include <QDialog>

class MaskDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MaskDialog(QWidget *parent = nullptr);
    ~MaskDialog() override;

    void setImageName(const QString &name);

    void setPixmap(const QPixmap& pixmap);
    auto maskImage() -> QImage;
    void setMaskImage(const QImage& image);

private slots:
    void onButtonClicked(int id);
    void onPenSizeChanged(int value);
    void onOpacityChanged(double value);
    void onSave();

private:
    void setupUI();
    void buildConnect();

    class MaskDialogPrivate;
    QScopedPointer<MaskDialogPrivate> d_ptr;
};

#endif // MASKDIALOG_H

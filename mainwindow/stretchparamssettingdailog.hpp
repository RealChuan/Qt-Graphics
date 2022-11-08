#ifndef STRETCHPARAMSSETTINGDAILOG_HPP
#define STRETCHPARAMSSETTINGDAILOG_HPP

#include <QDialog>

class StretchParamsSettingDailog : public QDialog
{
    Q_OBJECT
public:
    struct StretchParams
    {
        QSize size = QSize(0, 0);
        Qt::AspectRatioMode mode = Qt::KeepAspectRatio;
        int quality = -1;
    };

    explicit StretchParamsSettingDailog(QWidget *parent = nullptr);
    ~StretchParamsSettingDailog();

    void setParams(const StretchParams &params);
    StretchParams params() const;

private slots:
    void onReset();
    void onApply();

private:
    void setupUI();

    class StretchParamsSettingDailogPrivate;
    QScopedPointer<StretchParamsSettingDailogPrivate> d_ptr;
};

#endif // STRETCHPARAMSSETTINGDAILOG_HPP

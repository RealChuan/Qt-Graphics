#include "stretchparamssettingdailog.hpp"

#include <utils/validator.hpp>

#include <QtWidgets>

class StretchParamsSettingDailog::StretchParamsSettingDailogPrivate
{
public:
    StretchParamsSettingDailogPrivate(QWidget *parent)
        : q_ptr(parent)
    {
        widthLineEdit = new QLineEdit(q_ptr);
        widthLineEdit->setValidator(new QIntValidator(widthLineEdit));
        heightLineEdit = new QLineEdit(q_ptr);
        heightLineEdit->setValidator(new QIntValidator(heightLineEdit));
        aspectRatioModeComboBox = new QComboBox(q_ptr);
        const QMetaEnum aspectRatioMod = QMetaEnum::fromType<Qt::AspectRatioMode>();
        for (int i = 0; i < aspectRatioMod.keyCount(); i++) {
            aspectRatioModeComboBox->addItem(aspectRatioMod.key(i), aspectRatioMod.value(i));
        }
        qualityLineEdit = new QLineEdit(q_ptr);
        qualityLineEdit->setValidator(new Utils::IntValidator(-1, 100, qualityLineEdit));
    }

    QWidget *q_ptr;
    QLineEdit *widthLineEdit;
    QLineEdit *heightLineEdit;
    QComboBox *aspectRatioModeComboBox;
    QLineEdit *qualityLineEdit;

    StretchParamsSettingDailog::StretchParams params;
};

StretchParamsSettingDailog::StretchParamsSettingDailog(QWidget *parent)
    : QDialog(parent)
    , d_ptr(new StretchParamsSettingDailogPrivate(this))
{
    setWindowTitle(tr("Stretch Params"));
    setupUI();
    show();
}

StretchParamsSettingDailog::~StretchParamsSettingDailog() {}

void StretchParamsSettingDailog::setParams(const StretchParams &params)
{
    d_ptr->params = params;
    onReset();
}

StretchParamsSettingDailog::StretchParams StretchParamsSettingDailog::params() const
{
    return d_ptr->params;
}

void StretchParamsSettingDailog::onReset()
{
    d_ptr->widthLineEdit->setText(QString::number(d_ptr->params.size.width()));
    d_ptr->heightLineEdit->setText(QString::number(d_ptr->params.size.height()));
    d_ptr->aspectRatioModeComboBox->setCurrentIndex(
        d_ptr->aspectRatioModeComboBox->findData(d_ptr->params.mode));
    d_ptr->qualityLineEdit->setText(QString::number(d_ptr->params.quality));
    adjustSize();
}

void StretchParamsSettingDailog::onApply()
{
    d_ptr->params.size = QSize(d_ptr->widthLineEdit->text().trimmed().toInt(),
                               d_ptr->heightLineEdit->text().trimmed().toInt());
    d_ptr->params.mode = Qt::AspectRatioMode(d_ptr->aspectRatioModeComboBox->currentData().toInt());
    d_ptr->params.quality = d_ptr->qualityLineEdit->text().trimmed().toInt();
    accept();
}

void StretchParamsSettingDailog::setupUI()
{
    QPushButton *resetButton = new QPushButton(tr("Reset"), this);
    connect(resetButton, &QPushButton::clicked, this, &StretchParamsSettingDailog::onReset);
    QPushButton *applyButton = new QPushButton(tr("Apply"), this);
    connect(applyButton, &QPushButton::clicked, this, &StretchParamsSettingDailog::onApply);

    QFormLayout *layout = new QFormLayout(this);
    layout->addRow(tr("Width:"), d_ptr->widthLineEdit);
    layout->addRow(tr("Height:"), d_ptr->heightLineEdit);
    layout->addRow(tr("AspectRatioMod:"), d_ptr->aspectRatioModeComboBox);
    layout->addRow(tr("Quality(-1~100):"), d_ptr->qualityLineEdit);
    layout->addRow(resetButton, applyButton);
}

#include "stretchparamssettingdailog.hpp"

#include <QtWidgets>

class StretchParamsSettingDailog::StretchParamsSettingDailogPrivate
{
public:
    StretchParamsSettingDailogPrivate(QWidget *parent)
        : owner(parent)
    {
        widthLineEdit = new QLineEdit(owner);
        widthLineEdit->setValidator(new QIntValidator(widthLineEdit));
        heightLineEdit = new QLineEdit(owner);
        heightLineEdit->setValidator(new QIntValidator(heightLineEdit));
        aspectRatioModeComboBox = new QComboBox(owner);

        const QMetaEnum aspectRatioMod = QMetaEnum::fromType<Qt::AspectRatioMode>();
        for (int i = 0; i < aspectRatioMod.keyCount(); i++)
            aspectRatioModeComboBox->addItem(aspectRatioMod.key(i), aspectRatioMod.value(i));
    }

    QWidget *owner;
    QLineEdit *widthLineEdit;
    QLineEdit *heightLineEdit;
    QComboBox *aspectRatioModeComboBox;

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
    adjustSize();
}

void StretchParamsSettingDailog::onApply()
{
    d_ptr->params.size = QSize(d_ptr->widthLineEdit->text().trimmed().toInt(),
                               d_ptr->heightLineEdit->text().trimmed().toInt());
    d_ptr->params.mode = Qt::AspectRatioMode(d_ptr->aspectRatioModeComboBox->currentData().toInt());
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
    layout->addRow(resetButton, applyButton);
}

#include "bilateralfilter.hpp"

#include <utils/utils.h>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class BilateralFilter::BilateralFilterPrivate
{
public:
    explicit BilateralFilterPrivate(BilateralFilter *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(BilateralFilter::tr("Bilateral Filter"));

        diameterLabel = new QLabel(groupBox);
        diameterSlider = new QSlider(Qt::Horizontal, groupBox);
        diameterSlider->setRange(0, 20);
        diameterSlider->setTickPosition(QSlider::TicksBelow);
        diameterSlider->setTickInterval(1);

        sigmaColorSpinBox = new QDoubleSpinBox(groupBox);
        sigmaColorSpinBox->setRange(0.0, 200.0);
        sigmaColorSpinBox->setSingleStep(0.5);
        sigmaColorSpinBox->setValue(25.0);

        sigmaSpaceSpinBox = new QDoubleSpinBox(groupBox);
        sigmaSpaceSpinBox->setRange(0.0, 200.0);
        sigmaSpaceSpinBox->setSingleStep(0.5);
        sigmaSpaceSpinBox->setValue(25.0);

        borderTypeComboBox = new QComboBox(groupBox);
        borderTypeComboBox->addItem("CONSTANT", cv::BORDER_CONSTANT);
        borderTypeComboBox->addItem("REPLICATE", cv::BORDER_REPLICATE);
        borderTypeComboBox->addItem("REFLECT", cv::BORDER_REFLECT);
        borderTypeComboBox->addItem("WRAP", cv::BORDER_WRAP);
        borderTypeComboBox->addItem("REFLECT_1010", cv::BORDER_REFLECT_101);
        borderTypeComboBox->addItem("TRANSPARENT", cv::BORDER_TRANSPARENT);
        borderTypeComboBox->addItem("REFLECT101", cv::BORDER_REFLECT_101);
        borderTypeComboBox->addItem("DEFAULT", cv::BORDER_DEFAULT);
        borderTypeComboBox->addItem("ISOLATED", cv::BORDER_ISOLATED);
        borderTypeComboBox->setCurrentText("DEFAULT");
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(diameterLabel, diameterSlider);
        fromLayout->addRow(BilateralFilter::tr("Sigma Color:"), sigmaColorSpinBox);
        fromLayout->addRow(BilateralFilter::tr("Sigma Space:"), sigmaSpaceSpinBox);
        fromLayout->addRow(BilateralFilter::tr("Border Type:"), borderTypeComboBox);
    }

    BilateralFilter *q_ptr;

    QGroupBox *groupBox;
    QLabel *diameterLabel;
    QSlider *diameterSlider;
    QDoubleSpinBox *sigmaColorSpinBox;
    QDoubleSpinBox *sigmaSpaceSpinBox;
    QComboBox *borderTypeComboBox;
};

BilateralFilter::BilateralFilter(QObject *parent)
    : Filter(parent)
    , d_ptr(new BilateralFilterPrivate(this))
{
    d_ptr->setupUI();
    buildConnect();
}

BilateralFilter::~BilateralFilter() {}

auto BilateralFilter::canApply() const -> bool
{
    return true;
}

auto BilateralFilter::apply(const cv::Mat &src) -> cv::Mat
{
    auto diameter = d_ptr->diameterSlider->value();
    auto sigmaColor = d_ptr->sigmaColorSpinBox->value();
    auto sigmaSpace = d_ptr->sigmaSpaceSpinBox->value();
    auto borderType = d_ptr->borderTypeComboBox->currentData().toInt();

    return Utils::asynchronous<cv::Mat>(
        [src, diameter, sigmaColor, sigmaSpace, borderType]() -> cv::Mat {
            cv::Mat dst;
            try {
                cv::bilateralFilter(src, dst, diameter, sigmaColor, sigmaSpace, borderType);
            } catch (const cv::Exception &e) {
                qWarning() << "BilateralFilter:" << e.what();
            }
            return dst;
        });
}

auto BilateralFilter::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

void BilateralFilter::buildConnect()
{
    connect(d_ptr->diameterSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->diameterLabel->setText(tr("Diameter: %1").arg(value));
    });
    d_ptr->diameterSlider->setValue(5);
}

} // namespace OpenCVUtils

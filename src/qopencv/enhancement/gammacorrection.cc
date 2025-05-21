#include "gammacorrection.hpp"

#include <utils/utils.h>

#include <QtWidgets>

#include <opencv2/core.hpp>

namespace OpenCVUtils {

class GammaCorrection::GammaCorrectionPrivate
{
public:
    explicit GammaCorrectionPrivate(GammaCorrection *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(GammaCorrection::tr("Gamma Correction"));

        gammaSpinBox = new QDoubleSpinBox(groupBox);
        gammaSpinBox->setRange(0.1, 3.0);
        gammaSpinBox->setSingleStep(0.1);
        gammaSpinBox->setValue(1.0);
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(GammaCorrection::tr("Gamma:"), gammaSpinBox);
    }

    GammaCorrection *q_ptr;

    QGroupBox *groupBox;
    QDoubleSpinBox *gammaSpinBox;
};

GammaCorrection::GammaCorrection(QObject *parent)
    : Enhancement(parent)
    , d_ptr(new GammaCorrectionPrivate(this))
{
    d_ptr->setupUI();
}

GammaCorrection::~GammaCorrection() {}

auto GammaCorrection::canApply() const -> bool
{
    return true;
}

auto GammaCorrection::apply(const cv::Mat &src) -> cv::Mat
{
    auto gamma = d_ptr->gammaSpinBox->value();
    return Utils::asynchronous<cv::Mat>([src, gamma]() -> cv::Mat {
        cv::Mat dst;
        try {
            src.convertTo(dst, CV_32F, 1.0 / 255.0);
            cv::pow(dst, gamma, dst);
            dst.convertTo(dst, CV_8U, 255.0);
        } catch (const cv::Exception &e) {
            qWarning() << "GammaCorrection:" << e.what();
        }
        return dst;
    });
}

auto GammaCorrection::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

} // namespace OpenCVUtils

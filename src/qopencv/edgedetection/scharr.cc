#include "scharr.hpp"

#include <utils/utils.hpp>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class Scharr::ScharrPrivate
{
public:
    explicit ScharrPrivate(Scharr *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(Scharr::tr("Scharr"));

        depthComboBox = new QComboBox(groupBox);
        depthComboBox->addItem("CV_16S", CV_16S);
        depthComboBox->addItem("CV_32F", CV_32F);
        depthComboBox->addItem("CV_64F", CV_64F);

        scaleSpinBox = new QDoubleSpinBox(groupBox);
        scaleSpinBox->setRange(0.0, 10.0);
        scaleSpinBox->setSingleStep(0.1);
        scaleSpinBox->setValue(1.0);

        deltaSpinBox = new QDoubleSpinBox(groupBox);
        deltaSpinBox->setRange(-10.0, 10.0);
        deltaSpinBox->setSingleStep(0.1);
        deltaSpinBox->setValue(0.0);

        borderTypeComboBox = new QComboBox(groupBox);
        borderTypeComboBox->addItem("CONSTANT", cv::BORDER_CONSTANT);
        borderTypeComboBox->addItem("REPLICATE", cv::BORDER_REPLICATE);
        borderTypeComboBox->addItem("REFLECT", cv::BORDER_REFLECT);
        // borderTypeComboBox->addItem("WRAP", cv::BORDER_WRAP);
        borderTypeComboBox->addItem("REFLECT_1010", cv::BORDER_REFLECT_101);
        borderTypeComboBox->addItem("TRANSPARENT", cv::BORDER_TRANSPARENT);
        borderTypeComboBox->addItem("REFLECT101", cv::BORDER_REFLECT_101);
        borderTypeComboBox->addItem("DEFAULT", cv::BORDER_DEFAULT);
        borderTypeComboBox->addItem("ISOLATED", cv::BORDER_ISOLATED);
        borderTypeComboBox->setCurrentText("DEFAULT");
    };

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(Scharr::tr("Depth"), depthComboBox);
        fromLayout->addRow(Scharr::tr("Scale"), scaleSpinBox);
        fromLayout->addRow(Scharr::tr("Delta"), deltaSpinBox);
        fromLayout->addRow(Scharr::tr("Border Type"), borderTypeComboBox);
    }

    Scharr *q_ptr;
    QGroupBox *groupBox;
    QComboBox *depthComboBox;
    QDoubleSpinBox *scaleSpinBox;
    QDoubleSpinBox *deltaSpinBox;
    QComboBox *borderTypeComboBox;
};

Scharr::Scharr(QObject *parent)
    : EdgeDetection(parent)
    , d_ptr(new ScharrPrivate(this))
{
    d_ptr->setupUI();
}

Scharr::~Scharr() {}

auto Scharr::canApply() const -> bool
{
    return true;
}

auto Scharr::apply(const cv::Mat &src) -> cv::Mat
{
    auto ddepth = d_ptr->depthComboBox->currentData().toInt();
    auto scale = d_ptr->scaleSpinBox->value();
    auto delta = d_ptr->deltaSpinBox->value();
    auto borderType = d_ptr->borderTypeComboBox->currentData().toInt();

    return Utils::asynchronous<cv::Mat>([src, ddepth, scale, delta, borderType]() -> cv::Mat {
        cv::Mat dst;
        cv::Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
        try {
            cv::Scharr(src, grad_x, ddepth, 1, 0, scale, delta, borderType);
            cv::Scharr(src, grad_y, ddepth, 0, 1, scale, delta, borderType);
            cv::convertScaleAbs(grad_x, abs_grad_x);
            cv::convertScaleAbs(grad_y, abs_grad_y);
            cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);
        } catch (const std::exception &e) {
            qWarning() << e.what();
        }
        return dst;
    });
}

auto Scharr::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

} // namespace OpenCVUtils

#include "sobel.hpp"

#include <utils/utils.hpp>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class Sobel::SobelPrivate
{
public:
    explicit SobelPrivate(Sobel *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(Sobel::tr("Sobel Filter"));

        depthComboBox = new QComboBox(groupBox);
        depthComboBox->addItem("CV_16S", CV_16S);
        depthComboBox->addItem("CV_32F", CV_32F);
        depthComboBox->addItem("CV_64F", CV_64F);

        kSizeComboBox = new QComboBox(groupBox);
        kSizeComboBox->addItem("1", 1);
        kSizeComboBox->addItem("3", 3);
        kSizeComboBox->addItem("5", 5);
        kSizeComboBox->addItem("7", 7);
        kSizeComboBox->setCurrentText("3");

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
        fromLayout->addRow(Sobel::tr("Depth:"), depthComboBox);
        fromLayout->addRow(Sobel::tr("Kernel Size:"), kSizeComboBox);
        fromLayout->addRow(Sobel::tr("Scale:"), scaleSpinBox);
        fromLayout->addRow(Sobel::tr("Delta:"), deltaSpinBox);
        fromLayout->addRow(Sobel::tr("Border Type:"), borderTypeComboBox);
    }

    Sobel *q_ptr;

    QGroupBox *groupBox;
    QComboBox *depthComboBox;
    QComboBox *kSizeComboBox;
    QDoubleSpinBox *scaleSpinBox;
    QDoubleSpinBox *deltaSpinBox;
    QComboBox *borderTypeComboBox;
};

Sobel::Sobel(QObject *parent)
    : EdgeDetection(parent)
    , d_ptr(new SobelPrivate(this))
{
    d_ptr->setupUI();
}

Sobel::~Sobel() {}

auto Sobel::canApply() const -> bool
{
    return true;
}

auto Sobel::apply(const cv::Mat &src) -> cv::Mat
{
    auto ddepth = d_ptr->depthComboBox->currentData().toInt();
    auto ksize = d_ptr->kSizeComboBox->currentData().toInt();
    auto scale = d_ptr->scaleSpinBox->value();
    auto delta = d_ptr->deltaSpinBox->value();
    auto borderType = d_ptr->borderTypeComboBox->currentData().toInt();

    return Utils::asynchronous<cv::Mat>([src, ddepth, ksize, scale, delta, borderType]() -> cv::Mat {
        cv::Mat dst;
        cv::Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
        try {
            cv::Sobel(src, grad_x, ddepth, 1, 0, ksize, scale, delta, borderType);
            cv::Sobel(src, grad_y, ddepth, 0, 1, ksize, scale, delta, borderType);
            cv::convertScaleAbs(grad_x, abs_grad_x);
            cv::convertScaleAbs(grad_y, abs_grad_y);
            cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);
        } catch (const std::exception &e) {
            qDebug() << e.what();
        }
        return dst;
    });
}

auto Sobel::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

} // namespace OpenCVUtils

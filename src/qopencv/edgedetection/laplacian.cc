#include "laplacian.hpp"

#include <utils/utils.hpp>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class Laplacian::LaplacianPrivate
{
public:
    explicit LaplacianPrivate(Laplacian *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(Laplacian::tr("Laplacian Filter"));

        depthComboBox = new QComboBox(groupBox);
        depthComboBox->addItem("CV_16S", CV_16S);
        depthComboBox->addItem("CV_32F", CV_32F);
        depthComboBox->addItem("CV_64F", CV_64F);

        kSizeComboBox = new QComboBox(groupBox);
        for (int i = 1; i <= 31; i += 2) {
            kSizeComboBox->addItem(QString::number(i));
        }

        scaleSpinBox = new QDoubleSpinBox(groupBox);
        scaleSpinBox->setRange(0.0, 10.0);
        scaleSpinBox->setSingleStep(0.1);
        scaleSpinBox->setValue(1.0);

        deltaSpinBox = new QDoubleSpinBox(groupBox);
        deltaSpinBox->setRange(0.0, 10.0);
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
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(Laplacian::tr("Depth:"), depthComboBox);
        fromLayout->addRow(Laplacian::tr("Kernel Size:"), kSizeComboBox);
        fromLayout->addRow(Laplacian::tr("Scale:"), scaleSpinBox);
        fromLayout->addRow(Laplacian::tr("Delta:"), deltaSpinBox);
        fromLayout->addRow(Laplacian::tr("Border Type:"), borderTypeComboBox);
    }

    Laplacian *q_ptr;

    QGroupBox *groupBox;
    QComboBox *depthComboBox;
    QComboBox *kSizeComboBox;
    QDoubleSpinBox *scaleSpinBox;
    QDoubleSpinBox *deltaSpinBox;
    QComboBox *borderTypeComboBox;
};

Laplacian::Laplacian(QObject *parent)
    : EdgeDetection(parent)
    , d_ptr(new LaplacianPrivate(this))
{
    d_ptr->setupUI();
}

Laplacian::~Laplacian() {}

auto Laplacian::canApply() const -> bool
{
    return true;
}

auto Laplacian::apply(const cv::Mat &src) -> cv::Mat
{
    auto ddepth = d_ptr->depthComboBox->currentData().toInt();
    auto ksize = d_ptr->kSizeComboBox->currentText().toInt();
    auto scale = d_ptr->scaleSpinBox->value();
    auto delta = d_ptr->deltaSpinBox->value();
    auto borderType = d_ptr->borderTypeComboBox->currentData().toInt();

    return Utils::asynchronous<cv::Mat>([src, ddepth, ksize, scale, delta, borderType]() -> cv::Mat {
        cv::Mat dst;
        cv::Mat grad;
        try {
            cv::Laplacian(src, grad, ddepth, ksize, scale, delta, borderType);
            cv::convertScaleAbs(grad, dst);
        } catch (const std::exception &e) {
            qWarning() << e.what();
        }
        return dst;
    });
}

auto Laplacian::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

} // namespace OpenCVUtils

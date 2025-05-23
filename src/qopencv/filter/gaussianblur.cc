#include "gaussianblur.hpp"

#include <utils/utils.h>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class GaussianBlur::GaussianBlurPrivate
{
public:
    explicit GaussianBlurPrivate(GaussianBlur *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(GaussianBlur::tr("Gaussian Blur"));

        kWidthComboBox = new QComboBox(groupBox);
        kHeightComboBox = new QComboBox(groupBox);
        for (int i = 3; i <= 21; i += 2) {
            kWidthComboBox->addItem(QString::number(i), i);
            kHeightComboBox->addItem(QString::number(i), i);
        }

        sigmaXSpinBox = new QDoubleSpinBox(groupBox);
        sigmaXSpinBox->setRange(0.0, 20.0);
        sigmaXSpinBox->setSingleStep(0.5);
        sigmaXSpinBox->setValue(3);
        sigmaYSpinBox = new QDoubleSpinBox(groupBox);
        sigmaYSpinBox->setRange(0.0, 20.0);
        sigmaYSpinBox->setSingleStep(0.5);
        sigmaYSpinBox->setValue(3);

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

        hintComboBox = new QComboBox(groupBox);
        hintComboBox->addItem("DEFAULT", cv::ALGO_HINT_DEFAULT);
        hintComboBox->addItem("ACCURATE", cv::ALGO_HINT_ACCURATE);
        hintComboBox->addItem("APPROX", cv::ALGO_HINT_APPROX);
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(GaussianBlur::tr("Kernel Width:"), kWidthComboBox);
        fromLayout->addRow(GaussianBlur::tr("Kernel Height:"), kHeightComboBox);
        fromLayout->addRow(GaussianBlur::tr("Sigma X:"), sigmaXSpinBox);
        fromLayout->addRow(GaussianBlur::tr("Sigma Y:"), sigmaYSpinBox);
        fromLayout->addRow(GaussianBlur::tr("Border Type:"), borderTypeComboBox);
        fromLayout->addRow(GaussianBlur::tr("Algorithm Hint:"), hintComboBox);
    }

    GaussianBlur *q_ptr;

    QGroupBox *groupBox;
    QComboBox *kWidthComboBox;
    QComboBox *kHeightComboBox;
    QDoubleSpinBox *sigmaXSpinBox;
    QDoubleSpinBox *sigmaYSpinBox;
    QComboBox *borderTypeComboBox;
    QComboBox *hintComboBox;
};

GaussianBlur::GaussianBlur(QObject *parent)
    : Filter(parent)
    , d_ptr(new GaussianBlurPrivate(this))
{
    d_ptr->setupUI();
}

GaussianBlur::~GaussianBlur() {}

cv::Mat GaussianBlur::apply(const cv::Mat &src)
{
    cv::Size ksize(d_ptr->kWidthComboBox->currentData().toInt(),
                   d_ptr->kHeightComboBox->currentData().toInt());
    auto sigmaX = d_ptr->sigmaXSpinBox->value();
    auto sigmaY = d_ptr->sigmaYSpinBox->value();
    auto borderType = d_ptr->borderTypeComboBox->currentData().toInt();
    auto algoHint = static_cast<cv::AlgorithmHint>(d_ptr->hintComboBox->currentData().toInt());

    return Utils::asynchronous<cv::Mat>(
        [src, ksize, sigmaX, sigmaY, borderType, algoHint]() -> cv::Mat {
            cv::Mat dst;
            try {
                cv::GaussianBlur(src, dst, ksize, sigmaX, sigmaY, borderType, algoHint);
            } catch (const cv::Exception &e) {
                qWarning() << "GaussianBlur:" << e.what();
            }
            return dst;
        });
}

auto GaussianBlur::canApply() const -> bool
{
    return true;
}

auto GaussianBlur::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

} // namespace OpenCVUtils

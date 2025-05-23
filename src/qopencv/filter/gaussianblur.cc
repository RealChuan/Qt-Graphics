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

        kWidthLabel = new QLabel(groupBox);
        kWidthSlider = new QSlider(Qt::Horizontal, groupBox);
        kWidthSlider->setRange(1, 99);
        kWidthSlider->setTickPosition(QSlider::TicksBelow);
        kWidthSlider->setTickInterval(2);

        kHeightLabel = new QLabel(GaussianBlur::tr("Kernel Height:"), groupBox);
        kHeightSlider = new QSlider(Qt::Horizontal, groupBox);
        kHeightSlider->setRange(1, 99);
        kHeightSlider->setTickPosition(QSlider::TicksBelow);
        kHeightSlider->setTickInterval(2);

        sigmaXSpinBox = new QDoubleSpinBox(groupBox);
        sigmaXSpinBox->setRange(0.0, 20.0);
        sigmaXSpinBox->setSingleStep(0.5);
        sigmaXSpinBox->setValue(1.5);
        sigmaYSpinBox = new QDoubleSpinBox(groupBox);
        sigmaYSpinBox->setRange(0.0, 20.0);
        sigmaYSpinBox->setSingleStep(0.5);
        sigmaYSpinBox->setValue(1.5);

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

        hintComboBox = new QComboBox(groupBox);
        hintComboBox->addItem("DEFAULT", cv::ALGO_HINT_DEFAULT);
        hintComboBox->addItem("ACCURATE", cv::ALGO_HINT_ACCURATE);
        hintComboBox->addItem("APPROX", cv::ALGO_HINT_APPROX);
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(kWidthLabel, kWidthSlider);
        fromLayout->addRow(kHeightLabel, kHeightSlider);
        fromLayout->addRow(GaussianBlur::tr("Sigma X:"), sigmaXSpinBox);
        fromLayout->addRow(GaussianBlur::tr("Sigma Y:"), sigmaYSpinBox);
        fromLayout->addRow(GaussianBlur::tr("Border Type:"), borderTypeComboBox);
        fromLayout->addRow(GaussianBlur::tr("Algorithm Hint:"), hintComboBox);
    }

    GaussianBlur *q_ptr;

    QGroupBox *groupBox;
    QLabel *kWidthLabel;
    QSlider *kWidthSlider;
    QLabel *kHeightLabel;
    QSlider *kHeightSlider;
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
    buildConnect();
}

GaussianBlur::~GaussianBlur() {}

cv::Mat GaussianBlur::apply(const cv::Mat &src)
{
    cv::Size ksize(d_ptr->kWidthSlider->value(), d_ptr->kHeightSlider->value());
    if (ksize.width % 2 == 0) {
        ksize.width += 1;
    }
    if (ksize.height % 2 == 0) {
        ksize.height += 1;
    }
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

void GaussianBlur::buildConnect()
{
    connect(d_ptr->kWidthSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->kWidthLabel->setText(tr("Kernel Width: %1").arg(value));
    });
    d_ptr->kWidthSlider->setValue(3);
    connect(d_ptr->kHeightSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->kHeightLabel->setText(tr("Kernel Height: %1").arg(value));
    });
    d_ptr->kHeightSlider->setValue(3);
}

} // namespace OpenCVUtils

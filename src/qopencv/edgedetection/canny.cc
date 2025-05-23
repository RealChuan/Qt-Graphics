#include "canny.hpp"

#include <utils/utils.h>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class Canny::CannyPrivate
{
public:
    explicit CannyPrivate(Canny *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(Canny::tr("Canny Edge Detection"));

        lowThresholdLabel = new QLabel(groupBox);
        lowThresholdSlider = new QSlider(Qt::Horizontal, groupBox);
        lowThresholdSlider->setRange(0, 255);
        lowThresholdSlider->setTickPosition(QSlider::TicksBelow);
        lowThresholdSlider->setTickInterval(1);

        highThresholdLabel = new QLabel(groupBox);
        highThresholdSlider = new QSlider(Qt::Horizontal, groupBox);
        highThresholdSlider->setRange(0, 255);
        highThresholdSlider->setTickPosition(QSlider::TicksBelow);
        highThresholdSlider->setTickInterval(1);

        apertureSizeComboBox = new QComboBox(groupBox);
        apertureSizeComboBox->addItem("3", 3);
        apertureSizeComboBox->addItem("5", 5);
        apertureSizeComboBox->addItem("7", 7);

        l2GradientCheckBox = new QCheckBox(groupBox);
    }

    void setupUI()
    {
        auto *formLayout = new QFormLayout(groupBox);
        formLayout->addRow(lowThresholdLabel, lowThresholdSlider);
        formLayout->addRow(highThresholdLabel, highThresholdSlider);
        formLayout->addRow(Canny::tr("Aperture Size:"), apertureSizeComboBox);
        formLayout->addRow(Canny::tr("L2 Gradient:"), l2GradientCheckBox);
    }

    ~CannyPrivate() {}

    Canny *q_ptr;
    QGroupBox *groupBox;
    QLabel *lowThresholdLabel;
    QSlider *lowThresholdSlider;
    QLabel *highThresholdLabel;
    QSlider *highThresholdSlider;
    QComboBox *apertureSizeComboBox;
    QCheckBox *l2GradientCheckBox;
};

Canny::Canny(QObject *parent)
    : EdgeDetection(parent)
    , d_ptr(new CannyPrivate(this))
{
    d_ptr->setupUI();
    buildConnect();
}

Canny::~Canny() {}

auto Canny::canApply() const -> bool
{
    return true;
}

auto Canny::apply(const cv::Mat &src) -> cv::Mat
{
    auto lowThreshold = d_ptr->lowThresholdSlider->value();
    auto highThreshold = d_ptr->highThresholdSlider->value();
    auto apertureSize = d_ptr->apertureSizeComboBox->currentData().toInt();
    auto l2Gradient = d_ptr->l2GradientCheckBox->isChecked();

    return Utils::asynchronous<cv::Mat>(
        [src, lowThreshold, highThreshold, apertureSize, l2Gradient]() -> cv::Mat {
            cv::Mat dst;
            try {
                cv::Canny(src, dst, lowThreshold, highThreshold, apertureSize, l2Gradient);
            } catch (const std::exception &e) {
                qDebug() << "Canny Edge Detection Error:" << e.what();
            }
            return dst;
        });
}

auto Canny::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

void Canny::buildConnect()
{
    connect(d_ptr->lowThresholdSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->lowThresholdLabel->setText(Canny::tr("Low Threshold: %1").arg(value));
    });
    connect(d_ptr->highThresholdSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->highThresholdLabel->setText(Canny::tr("High Threshold: %1").arg(value));
    });
    d_ptr->lowThresholdSlider->setValue(100);
    d_ptr->highThresholdSlider->setValue(200);
}

} // namespace OpenCVUtils

#include "adaptivethreshold.hpp"

#include <utils/utils.hpp>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class AdaptiveThreshold::AdaptiveThresholdPrivate
{
public:
    explicit AdaptiveThresholdPrivate(AdaptiveThreshold *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(AdaptiveThreshold::tr("Adaptive Threshold"));

        maxValueLabel = new QLabel(groupBox);
        maxValueSlider = new QSlider(Qt::Horizontal, groupBox);
        maxValueSlider->setRange(0, 255);
        maxValueSlider->setTickPosition(QSlider::TicksBelow);
        maxValueSlider->setTickInterval(1);

        adaptiveMethodComboBox = new QComboBox(groupBox);
        adaptiveMethodComboBox->addItem("Mean", cv::ADAPTIVE_THRESH_MEAN_C);
        adaptiveMethodComboBox->addItem("Gaussian", cv::ADAPTIVE_THRESH_GAUSSIAN_C);

        thresholdTypeComboBox = new QComboBox(groupBox);
        thresholdTypeComboBox->addItem("Binary", cv::THRESH_BINARY);
        thresholdTypeComboBox->addItem("Binary Inverted", cv::THRESH_BINARY_INV);

        blockSizeComboBox = new QComboBox(groupBox);
        for (int i = 3; i <= 21; i += 2) {
            blockSizeComboBox->addItem(QString::number(i), i);
        }
        blockSizeComboBox->setCurrentText(QString::number(9));

        constantLabel = new QLabel(groupBox);
        constantSlider = new QSlider(Qt::Horizontal, groupBox);
        constantSlider->setRange(-255, 255);
        constantSlider->setTickPosition(QSlider::TicksBelow);
        constantSlider->setTickInterval(1);
    }

    void setupUI()
    {
        auto *formLayout = new QFormLayout(groupBox);
        formLayout->addRow(maxValueLabel, maxValueSlider);
        formLayout->addRow(AdaptiveThreshold::tr("Adaptive Method:"), adaptiveMethodComboBox);
        formLayout->addRow(AdaptiveThreshold::tr("Threshold Type:"), thresholdTypeComboBox);
        formLayout->addRow(AdaptiveThreshold::tr("Block Size:"), blockSizeComboBox);
        formLayout->addRow(constantLabel, constantSlider);
    }

    AdaptiveThreshold *q_ptr;

    QGroupBox *groupBox;
    QLabel *maxValueLabel;
    QSlider *maxValueSlider;
    QComboBox *adaptiveMethodComboBox;
    QComboBox *thresholdTypeComboBox;
    QComboBox *blockSizeComboBox;
    QLabel *constantLabel;
    QSlider *constantSlider;
};

AdaptiveThreshold::AdaptiveThreshold(QObject *parent)
    : Segmentation(parent)
    , d_ptr(new AdaptiveThresholdPrivate(this))
{
    d_ptr->setupUI();
    buildConnect();
}

AdaptiveThreshold::~AdaptiveThreshold() {}

auto AdaptiveThreshold::canApply() const -> bool
{
    return true;
}

auto AdaptiveThreshold::apply(const cv::Mat &src) -> cv::Mat
{
    auto maxValue = d_ptr->maxValueSlider->value();
    auto adaptiveMethod = d_ptr->adaptiveMethodComboBox->currentData().toInt();
    auto thresholdType = d_ptr->thresholdTypeComboBox->currentData().toInt();
    auto blockSize = d_ptr->blockSizeComboBox->currentData().toInt();
    auto constant = d_ptr->constantSlider->value();

    return Utils::asynchronous<cv::Mat>(
        [src, maxValue, adaptiveMethod, thresholdType, blockSize, constant]() -> cv::Mat {
            cv::Mat dst;
            cv::Mat gray;
            try {
                cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
                cv::adaptiveThreshold(gray,
                                      dst,
                                      maxValue,
                                      adaptiveMethod,
                                      thresholdType,
                                      blockSize,
                                      constant);
            } catch (const std::exception &e) {
                qWarning() << "AdaptiveThreshold:" << e.what();
            }
            return dst;
        });
}

auto AdaptiveThreshold::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

void AdaptiveThreshold::buildConnect()
{
    connect(d_ptr->maxValueSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->maxValueLabel->setText(tr("Max Value: %1").arg(value));
    });
    d_ptr->maxValueSlider->setValue(255);

    connect(d_ptr->constantSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->constantLabel->setText(tr("Constant: %1").arg(value));
    });
    d_ptr->constantSlider->setValue(9);
}

} // namespace OpenCVUtils

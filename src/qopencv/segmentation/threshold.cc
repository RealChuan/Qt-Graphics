#include "threshold.hpp"

#include <utils/utils.hpp>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class Threshold::ThresholdPrivate
{
public:
    explicit ThresholdPrivate(Threshold *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(Threshold::tr("Threshold"));

        thresholdLabel = new QLabel(groupBox);
        thresholdSlider = new QSlider(Qt::Horizontal, groupBox);
        thresholdSlider->setRange(0, 255);
        thresholdSlider->setTickPosition(QSlider::TicksBelow);
        thresholdSlider->setTickInterval(1);

        maxValueLabel = new QLabel(groupBox);
        maxValueSlider = new QSlider(Qt::Horizontal, groupBox);
        maxValueSlider->setRange(0, 255);
        maxValueSlider->setTickPosition(QSlider::TicksBelow);
        maxValueSlider->setTickInterval(1);

        typeComboBox = new QComboBox(groupBox);
        typeComboBox->addItem("Binary", cv::THRESH_BINARY);
        typeComboBox->addItem("Binary Inverted", cv::THRESH_BINARY_INV);
        typeComboBox->addItem("Truncate", cv::THRESH_TRUNC);
        typeComboBox->addItem("To Zero", cv::THRESH_TOZERO);
        typeComboBox->addItem("To Zero Inverted", cv::THRESH_TOZERO_INV);
        typeComboBox->addItem("Mask", cv::THRESH_MASK);
        auto count = typeComboBox->count();
        typeComboBox->addItem("OTSU", cv::THRESH_OTSU);
        typeComboBox->addItem("Triangle", cv::THRESH_TRIANGLE);

        for (int i = 0; i < count; ++i) {
            typeComboBox->addItem(typeComboBox->itemText(i) + " | OTSU",
                                  typeComboBox->itemData(i).toInt() | cv::THRESH_OTSU);
            typeComboBox->addItem(typeComboBox->itemText(i) + " | Triangle",
                                  typeComboBox->itemData(i).toInt() | cv::THRESH_TRIANGLE);
        }
    }

    void setupUI()
    {
        auto *formLayout = new QFormLayout(groupBox);
        formLayout->addRow(thresholdLabel, thresholdSlider);
        formLayout->addRow(maxValueLabel, maxValueSlider);
        formLayout->addRow(Threshold::tr("Type:"), typeComboBox);
    }

    Threshold *q_ptr;

    QGroupBox *groupBox;
    QLabel *thresholdLabel;
    QSlider *thresholdSlider;
    QLabel *maxValueLabel;
    QSlider *maxValueSlider;
    QComboBox *typeComboBox;
};

Threshold::Threshold(QObject *parent)
    : Segmentation(parent)
    , d_ptr(new ThresholdPrivate(this))
{
    d_ptr->setupUI();
    buildConnect();
}

Threshold::~Threshold() {}

auto Threshold::canApply() const -> bool
{
    return true;
}

auto Threshold::apply(const cv::Mat &src) -> cv::Mat
{
    auto threshold = d_ptr->thresholdSlider->value();
    auto maxValue = d_ptr->maxValueSlider->value();
    auto type = d_ptr->typeComboBox->currentData().toInt();

    return Utils::asynchronous<cv::Mat>([src, threshold, maxValue, type]() -> cv::Mat {
        cv::Mat copy = src;
        cv::Mat dst;
        try {
            if ((type & cv::THRESH_OTSU) == cv::THRESH_OTSU
                || (type & cv::THRESH_TRIANGLE) == cv::THRESH_TRIANGLE) {
                cv::Mat gray;
                cv::cvtColor(copy, gray, cv::COLOR_BGR2GRAY);
                copy = gray;
            }
            cv::threshold(copy, dst, threshold, maxValue, type);
        } catch (const std::exception &e) {
            qWarning() << "Threshold:" << e.what();
        }
        return dst;
    });
}

auto Threshold::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

void Threshold::buildConnect()
{
    connect(d_ptr->thresholdSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->thresholdLabel->setText(QString("Threshold: %1").arg(value));
    });
    d_ptr->thresholdSlider->setValue(128);
    connect(d_ptr->maxValueSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->maxValueLabel->setText(QString("Max Value: %1").arg(value));
    });
    d_ptr->maxValueSlider->setValue(255);
}

} // namespace OpenCVUtils

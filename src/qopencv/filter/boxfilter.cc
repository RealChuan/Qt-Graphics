#include "boxfilter.hpp"

#include <utils/utils.h>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class BoxFilter::BoxFilterPrivate
{
public:
    explicit BoxFilterPrivate(BoxFilter *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(BoxFilter::tr("Box Filter"));

        kWidthLabel = new QLabel(groupBox);
        kWidthSlider = new QSlider(Qt::Horizontal, groupBox);
        kWidthSlider->setRange(3, 21);
        kWidthSlider->setTickPosition(QSlider::TicksBelow);
        kWidthSlider->setTickInterval(2);

        kHeightLabel = new QLabel(groupBox);
        kHeightSlider = new QSlider(Qt::Horizontal, groupBox);
        kHeightSlider->setRange(3, 21);
        kHeightSlider->setTickPosition(QSlider::TicksBelow);
        kHeightSlider->setTickInterval(2);

        kNormalizeCheckBox = new QCheckBox(tr("Normalize"), groupBox);

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

        fromLayout->addRow(kWidthLabel, kWidthSlider);
        fromLayout->addRow(kHeightLabel, kHeightSlider);
        fromLayout->addWidget(kNormalizeCheckBox);
        fromLayout->addRow(BoxFilter::tr("Border Type:"), borderTypeComboBox);
    }

    BoxFilter *q_ptr;

    QGroupBox *groupBox;
    QLabel *kWidthLabel;
    QSlider *kWidthSlider;
    QLabel *kHeightLabel;
    QSlider *kHeightSlider;
    QCheckBox *kNormalizeCheckBox;
    QComboBox *borderTypeComboBox;
};

BoxFilter::BoxFilter(QObject *parent)
    : Filter(parent)
    , d_ptr(new BoxFilterPrivate(this))
{
    d_ptr->setupUI();
    buildConnect();
}

BoxFilter::~BoxFilter() {}

auto BoxFilter::canApply() const -> bool
{
    return true;
}

auto BoxFilter::apply(const cv::Mat &src) -> cv::Mat
{
    auto kWidth = d_ptr->kWidthSlider->value();
    if (kWidth % 2 == 0) {
        kWidth++;
    }
    auto kHeight = d_ptr->kHeightSlider->value();
    if (kHeight % 2 == 0) {
        kHeight++;
    }
    auto kNormalize = d_ptr->kNormalizeCheckBox->isChecked();
    auto borderType = d_ptr->borderTypeComboBox->currentData().toInt();

    return Utils::asynchronous<cv::Mat>([src, kWidth, kHeight, kNormalize, borderType]() -> cv::Mat {
        cv::Mat dst;
        try {
            cv::boxFilter(src,
                          dst,
                          src.depth(),
                          cv::Size(kWidth, kHeight),
                          cv::Point(-1, -1),
                          kNormalize,
                          borderType);
        } catch (const cv::Exception &e) {
            qWarning() << "BoxFilter:" << e.what();
        }
        return dst;
    });
}

auto BoxFilter::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

void BoxFilter::buildConnect()
{
    connect(d_ptr->kWidthSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->kWidthLabel->setText(tr("Width: %1").arg(value));
    });
    connect(d_ptr->kHeightSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->kHeightLabel->setText(tr("Height: %1").arg(value));
    });
    d_ptr->kWidthLabel->setText(tr("Width: %1").arg(d_ptr->kWidthSlider->value()));
    d_ptr->kHeightLabel->setText(tr("Height: %1").arg(d_ptr->kHeightSlider->value()));
}

} // namespace OpenCVUtils

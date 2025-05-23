#include "blur.hpp"

#include <utils/utils.h>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class Blur::BlurPrivate
{
public:
    explicit BlurPrivate(Blur *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(Blur::tr("Blur"));

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
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(kWidthLabel, kWidthSlider);
        fromLayout->addRow(kHeightLabel, kHeightSlider);
    }

    Blur *q_ptr;

    QGroupBox *groupBox;
    QLabel *kWidthLabel;
    QSlider *kWidthSlider;
    QLabel *kHeightLabel;
    QSlider *kHeightSlider;
};

Blur::Blur(QObject *parent)
    : Filter(parent)
    , d_ptr(new BlurPrivate(this))
{
    d_ptr->setupUI();
    buildConnect();
}

Blur::~Blur() {}

auto Blur::canApply() const -> bool
{
    return true;
}

auto Blur::apply(const cv::Mat &src) -> cv::Mat
{
    auto kWidth = d_ptr->kWidthSlider->value();
    if (kWidth % 2 == 0) {
        kWidth += 1;
    }
    auto kHeight = d_ptr->kHeightSlider->value();
    if (kHeight % 2 == 0) {
        kHeight += 1;
    }

    return Utils::asynchronous<cv::Mat>([src, kWidth, kHeight]() -> cv::Mat {
        cv::Mat dst;
        try {
            cv::blur(src, dst, cv::Size(kWidth, kHeight));
        } catch (const cv::Exception &e) {
            qWarning() << "Blur:" << e.what();
        }
        return dst;
    });
}

auto Blur::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

void Blur::buildConnect()
{
    connect(d_ptr->kWidthSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->kWidthLabel->setText(tr("Kernel Width: %1").arg(value));
    });
    connect(d_ptr->kHeightSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->kHeightLabel->setText(tr("Kernel Height: %1").arg(value));
    });

    d_ptr->kWidthLabel->setText(tr("Kernel Width: %1").arg(d_ptr->kWidthSlider->value()));
    d_ptr->kHeightLabel->setText(tr("Kernel Height: %1").arg(d_ptr->kHeightSlider->value()));
}

} // namespace OpenCVUtils

#include "linearcontrast.hpp"

#include <utils/utils.hpp>

#include <QtWidgets>

#include <opencv2/core.hpp>

namespace OpenCVUtils {

class LinearContrast::LinearContrastPrivate
{
public:
    explicit LinearContrastPrivate(LinearContrast *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(LinearContrast::tr("Linear Contrast"));

        alphaSpinBox = new QDoubleSpinBox(groupBox);
        alphaSpinBox->setRange(1.0, 3.0);
        alphaSpinBox->setSingleStep(0.1);
        alphaSpinBox->setValue(1.0);

        betaLabel = new QLabel(LinearContrast::tr("Beta:"), groupBox);
        betaSlider = new QSlider(Qt::Horizontal, groupBox);
        betaSlider->setRange(0, 100);
        betaSlider->setTickPosition(QSlider::TicksBelow);
        betaSlider->setTickInterval(1);
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(LinearContrast::tr("Alpha:"), alphaSpinBox);
        fromLayout->addRow(betaLabel, betaSlider);
    }

    LinearContrast *q_ptr;

    QGroupBox *groupBox;
    QDoubleSpinBox *alphaSpinBox;
    QLabel *betaLabel;
    QSlider *betaSlider;
};

LinearContrast::LinearContrast(QObject *parent)
    : Enhancement(parent)
    , d_ptr(new LinearContrastPrivate(this))
{
    d_ptr->setupUI();
    buildConnect();
}

LinearContrast::~LinearContrast() {}

auto LinearContrast::canApply() const -> bool
{
    return true;
}

auto LinearContrast::apply(const cv::Mat &src) -> cv::Mat
{
    auto alpha = d_ptr->alphaSpinBox->value();
    auto beta = d_ptr->betaSlider->value();

    return Utils::asynchronous<cv::Mat>([src, alpha, beta]() -> cv::Mat {
        cv::Mat dst;
        try {
            src.convertTo(dst, -1, alpha, beta);
        } catch (const cv::Exception &e) {
            qWarning() << "LinearContrast:" << e.what();
        }
        return dst;
    });
}

auto LinearContrast::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

void LinearContrast::buildConnect()
{
    connect(d_ptr->betaSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->betaLabel->setText(tr("Beta: %1").arg(value));
    });
    d_ptr->betaLabel->setText(tr("Beta: %1").arg(d_ptr->betaSlider->value()));
}

} // namespace OpenCVUtils

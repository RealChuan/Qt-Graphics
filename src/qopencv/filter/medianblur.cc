#include "medianblur.hpp"

#include <utils/utils.h>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class MedianBlur::MedianBlurPrivate
{
public:
    explicit MedianBlurPrivate(MedianBlur *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(MedianBlur::tr("Median Blur"));

        kSizeLabel = new QLabel(groupBox);
        kSizeSlider = new QSlider(Qt::Horizontal, groupBox);
        kSizeSlider->setRange(3, 15);
        kSizeSlider->setTickPosition(QSlider::TicksBelow);
        kSizeSlider->setTickInterval(2);
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(kSizeLabel, kSizeSlider);
    }

    MedianBlur *q_ptr;

    QGroupBox *groupBox;
    QLabel *kSizeLabel;
    QSlider *kSizeSlider;
};

MedianBlur::MedianBlur(QObject *parent)
    : Filter(parent)
    , d_ptr(new MedianBlurPrivate(this))
{
    d_ptr->setupUI();
    buildConnect();
}

MedianBlur::~MedianBlur() {}

auto MedianBlur::canApply() const -> bool
{
    return true;
}

auto MedianBlur::apply(const cv::Mat &src) -> cv::Mat
{
    auto ksize = d_ptr->kSizeSlider->value();
    if (ksize % 2 == 0) {
        ksize += 1;
    }

    return Utils::asynchronous<cv::Mat>([src, ksize]() -> cv::Mat {
        cv::Mat dst;
        try {
            cv::medianBlur(src, dst, ksize);
        } catch (const cv::Exception &e) {
            qWarning() << "MedianBlur:" << e.what();
        }
        return dst;
    });
}

auto MedianBlur::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

void MedianBlur::buildConnect()
{
    connect(d_ptr->kSizeSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->kSizeLabel->setText(MedianBlur::tr("Kernel Size: %1").arg(value));
    });
    d_ptr->kSizeLabel->setText(MedianBlur::tr("Kernel Size: %1").arg(d_ptr->kSizeSlider->value()));
}

} // namespace OpenCVUtils

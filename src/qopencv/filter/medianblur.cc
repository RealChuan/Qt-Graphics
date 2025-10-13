#include "medianblur.hpp"

#include <utils/utils.hpp>

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

        kSizeComboBox = new QComboBox(groupBox);
        for (int i = 3; i <= 21; i += 2) {
            kSizeComboBox->addItem(QString::number(i), i);
        }
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(MedianBlur::tr("Kernel Size:"), kSizeComboBox);
    }

    MedianBlur *q_ptr;

    QGroupBox *groupBox;
    QComboBox *kSizeComboBox;
};

MedianBlur::MedianBlur(QObject *parent)
    : Filter(parent)
    , d_ptr(new MedianBlurPrivate(this))
{
    d_ptr->setupUI();
}

MedianBlur::~MedianBlur() {}

auto MedianBlur::canApply() const -> bool
{
    return true;
}

auto MedianBlur::apply(const cv::Mat &src) -> cv::Mat
{
    auto ksize = d_ptr->kSizeComboBox->currentData().toInt();

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

} // namespace OpenCVUtils

#include "dehazed.hpp"

#include <utils/utils.h>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>
#include <opencv2/ximgproc/edge_filter.hpp>

namespace OpenCVUtils {

cv::Mat darkChannel(const cv::Mat &image, int patchSize = 15)
{
    // 转换为浮点型图像
    cv::Mat floatImage;
    image.convertTo(floatImage, CV_32F, 1.0 / 255.0);

    // 分离RGB通道
    std::vector<cv::Mat> channels(3);
    cv::split(floatImage, channels);

    // 计算每个像素的最小通道值（暗通道）
    cv::Mat dark;
    cv::min(channels[0], channels[1], dark);
    cv::min(dark, channels[2], dark);

    // 使用最小值滤波（局部窗口取最小值）
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(patchSize, patchSize));
    cv::erode(dark, dark, kernel); // 使用腐蚀操作近似最小值滤波

    return dark;
}

cv::Scalar estimateAtmosphericLight(const cv::Mat &image,
                                    const cv::Mat &darkChannel,
                                    double topPercent = 0.001)
{
    // 获取暗通道中最亮的前topPercent%像素
    int numPixels = darkChannel.rows * darkChannel.cols;
    int numTopPixels = static_cast<int>(numPixels * topPercent);
    std::vector<float> darkValues;
    for (int i = 0; i < darkChannel.rows; ++i) {
        const float *row = darkChannel.ptr<float>(i);
        for (int j = 0; j < darkChannel.cols; ++j) {
            darkValues.push_back(row[j]);
        }
    }

    // 排序并取前topPercent%的索引
    std::vector<int> indices(numPixels);
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return darkValues[a] > darkValues[b];
    });

    // 在原图中对应位置找最大值作为大气光
    cv::Scalar atmosphericLight(0, 0, 0);
    for (int i = 0; i < numTopPixels; ++i) {
        int idx = indices[i];
        int row = idx / image.cols;
        int col = idx % image.cols;
        cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);
        for (int c = 0; c < 3; ++c) {
            atmosphericLight[c] = std::max(atmosphericLight[c], static_cast<double>(pixel[c]));
        }
    }

    return atmosphericLight;
}

cv::Mat computeTransmission(const cv::Mat &darkChannel,
                            const cv::Scalar &atmosphericLight,
                            double omega = 0.95)
{
    // 归一化暗通道（假设大气光接近1.0）
    cv::Mat normalizedDark;
    darkChannel.copyTo(normalizedDark);
    for (int c = 0; c < 3; ++c) {
        normalizedDark.setTo(cv::Scalar::all(1.0), cv::Mat(normalizedDark < atmosphericLight[c]));
    }

    // 透射率公式：t = 1 - ω * dark_channel
    cv::Mat transmission;
    transmission = 1.0 - omega * normalizedDark;

    // 添加阈值防止除零错误
    cv::threshold(transmission, transmission, 0.1, 1.0, cv::THRESH_TRUNC);

    return transmission;
}

cv::Mat guidedFilterTransmission(const cv::Mat &transmission,
                                 const cv::Mat &image,
                                 int radius = 60,
                                 double epsilon = 0.001)
{
    // 1. 预处理：将引导图（原图像）转换为浮点型
    cv::Mat guide;
    image.convertTo(guide, CV_32F, 1.0 / 255.0); // 归一化到[0,1]

    // 2. 引导滤波
    cv::Mat refinedTransmission;
    cv::ximgproc::guidedFilter(guide, transmission, refinedTransmission, radius, epsilon);

    // 3. 后续处理（保持与原代码一致）
    cv::threshold(refinedTransmission,
                  refinedTransmission,
                  0.1,
                  1.0,
                  cv::THRESH_TRUNC); // 防止除零错误
    return refinedTransmission;
}

cv::Mat recoverImage(const cv::Mat &image,
                     const cv::Mat &transmission,
                     const cv::Scalar &atmosphericLight,
                     double t0 = 0.1)
{
    cv::Mat result = cv::Mat::zeros(image.size(), CV_32FC3);
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
            float t = transmission.at<float>(i, j);
            for (int c = 0; c < 3; ++c) {
                result.at<cv::Vec3f>(i, j)[c] = (pixel[c] - atmosphericLight[c])
                                                    / std::max(t, static_cast<float>(t0))
                                                + atmosphericLight[c];
            }
        }
    }

    // 归一化到[0,255]
    cv::normalize(result, result, 0, 255, cv::NORM_MINMAX);
    result.convertTo(result, CV_8UC3);
    return result;
}

class Dehazed::DehazedPrivate
{
public:
    explicit DehazedPrivate(Dehazed *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(Dehazed::tr("Dehazed"));

        patchSizeLabel = new QLabel(groupBox);
        patchSizeSlider = new QSlider(Qt::Horizontal, groupBox);
        patchSizeSlider->setRange(3, 25);
        patchSizeSlider->setTickPosition(QSlider::TicksBelow);
        patchSizeSlider->setTickInterval(2);

        omegaSpinBox = new QDoubleSpinBox(groupBox);
        omegaSpinBox->setRange(0.0, 1.0);
        omegaSpinBox->setSingleStep(0.01);
        omegaSpinBox->setValue(0.95);

        topPercentSpinBox = new QDoubleSpinBox(groupBox);
        topPercentSpinBox->setDecimals(3);
        topPercentSpinBox->setRange(0.001, 0.01);
        topPercentSpinBox->setSingleStep(0.001);
        topPercentSpinBox->setValue(0.001);
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(patchSizeLabel, patchSizeSlider);
        fromLayout->addRow(tr("Omega:"), omegaSpinBox);
        fromLayout->addRow(tr("Top Percent:"), topPercentSpinBox);
    }

    Dehazed *q_ptr;

    QGroupBox *groupBox;
    QLabel *patchSizeLabel;
    QSlider *patchSizeSlider;
    QDoubleSpinBox *omegaSpinBox;
    QDoubleSpinBox *topPercentSpinBox;
};

Dehazed::Dehazed(QObject *parent)
    : Enhancement(parent)
    , d_ptr(new DehazedPrivate(this))
{
    d_ptr->setupUI();
    buildConnect();
}

Dehazed::~Dehazed() {}

auto Dehazed::canApply() const -> bool
{
    return true;
}

auto Dehazed::apply(const cv::Mat &src) -> cv::Mat
{
    auto patchSize = d_ptr->patchSizeSlider->value();
    if (patchSize % 2 == 0) {
        patchSize += 1;
    }
    auto omega = d_ptr->omegaSpinBox->value();
    auto topPercent = d_ptr->topPercentSpinBox->value();

    return Utils::asynchronous<cv::Mat>([src, patchSize, omega, topPercent]() -> cv::Mat {
        cv::Mat dst;
        try {
            auto dark = darkChannel(src, patchSize);
            auto atmosphericLight = estimateAtmosphericLight(src, dark, topPercent);
            auto transmission = computeTransmission(dark, atmosphericLight, omega);
            auto refinedTransmission = guidedFilterTransmission(transmission, src);
            dst = recoverImage(src, refinedTransmission, atmosphericLight);
        } catch (const cv::Exception &e) {
            qWarning() << "Dehazed:" << e.what();
        }
        return dst;
    });
}

auto Dehazed::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

void Dehazed::buildConnect()
{
    connect(d_ptr->patchSizeSlider, &QSlider::valueChanged, this, [this](int value) {
        d_ptr->patchSizeLabel->setText(tr("Patch Size: %1").arg(value));
    });
    d_ptr->patchSizeSlider->setValue(15);
}

} // namespace OpenCVUtils

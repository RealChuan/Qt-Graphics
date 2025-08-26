#include "opencvutils.hpp"

#include <QDebug>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

struct FormatEntry
{
    QImage::Format unifiedFormat = QImage::Format_Invalid; // 统一后的 Qt 格式
    int openCvType = -1;                                   // OpenCV 数据类型
    int openCvCode = 0;                                    // 颜色/通道转换码；0 表示无需转换
};

constexpr auto kMatToQTable = [] {
    std::array<std::array<FormatEntry, 5>, 4> table{};

    auto set = [&](int depthIndex, int channelCount, QImage::Format unified, int code = 0) {
        table[depthIndex][channelCount] = {unified, -1, code};
    };

    /* 8-bit */
    set(0, 1, QImage::Format_Grayscale8);
    set(0, 3, QImage::Format_BGR888);
    set(0, 4, QImage::Format_ARGB32, cv::COLOR_BGRA2RGBA);

    /* 16-bit */
    set(1, 1, QImage::Format_Grayscale16);
    set(1, 3, QImage::Format_RGBX64);
    set(1, 4, QImage::Format_RGBA64, cv::COLOR_BGRA2RGBA);

    /* 32-bit float */
    set(2, 3, QImage::Format_RGBX32FPx4);
    set(2, 4, QImage::Format_RGBA32FPx4, cv::COLOR_BGRA2RGBA);

    return table;
}();

constexpr auto kQToMatTable = [] {
    constexpr int maxFormat = static_cast<int>(QImage::NImageFormats);
    std::array<FormatEntry, maxFormat> table{};

    auto set = [&](QImage::Format from, QImage::Format unified, int type, int swap = 0) {
        table[from] = {unified, type, swap};
    };

    /* 8-bit 灰度 */
    set(QImage::Format_Grayscale8, QImage::Format_Grayscale8, CV_8UC1);
    set(QImage::Format_Indexed8, QImage::Format_Grayscale8, CV_8UC1); // 降级为8位灰度

    /* 16-bit 灰度 */
    set(QImage::Format_Grayscale16, QImage::Format_Grayscale16, CV_16UC1);

    /* 8-bit 彩色 3 通道 */
    set(QImage::Format_RGB888, QImage::Format_BGR888, CV_8UC3, cv::COLOR_RGB2BGR);
    set(QImage::Format_BGR888, QImage::Format_BGR888, CV_8UC3);
    set(QImage::Format_RGB32, QImage::Format_BGR888, CV_8UC3, cv::COLOR_RGB2BGR);

    /* 8-bit 彩色 4 通道 */
    set(QImage::Format_ARGB32, QImage::Format_ARGB32, CV_8UC4, cv::COLOR_RGBA2BGRA);
    set(QImage::Format_ARGB32_Premultiplied, QImage::Format_ARGB32, CV_8UC4, cv::COLOR_RGBA2BGRA);
    set(QImage::Format_RGBA8888, QImage::Format_ARGB32, CV_8UC4, cv::COLOR_RGBA2BGRA);
    set(QImage::Format_RGBA8888_Premultiplied, QImage::Format_ARGB32, CV_8UC4, cv::COLOR_RGBA2BGRA);
    set(QImage::Format_RGBX8888, QImage::Format_BGR888, CV_8UC3, cv::COLOR_RGBA2BGR);

    /* 16-bit 彩色 */
    set(QImage::Format_RGB16, QImage::Format_BGR888, CV_8UC3, cv::COLOR_BGR5652BGR);
    set(QImage::Format_RGB555, QImage::Format_BGR888, CV_8UC3, cv::COLOR_BGR5552BGR);
    set(QImage::Format_RGB666, QImage::Format_BGR888, CV_8UC3, cv::COLOR_RGB2BGR);

    /* 16/32-bit 浮点 & 16-bit 整数 */
    set(QImage::Format_RGBX64, QImage::Format_RGBA64, CV_16UC3, cv::COLOR_RGB2BGR);
    set(QImage::Format_RGBA64, QImage::Format_RGBA64, CV_16UC4, cv::COLOR_RGBA2BGRA);
    set(QImage::Format_RGBA64_Premultiplied, QImage::Format_RGBA64, CV_16UC4, cv::COLOR_RGBA2BGRA);

    set(QImage::Format_RGBX16FPx4, QImage::Format_RGBX16FPx4, CV_16FC3, cv::COLOR_RGB2BGR);
    set(QImage::Format_RGBA16FPx4, QImage::Format_RGBA16FPx4, CV_16FC4, cv::COLOR_RGBA2BGRA);
    set(QImage::Format_RGBA16FPx4_Premultiplied,
        QImage::Format_RGBA16FPx4,
        CV_16FC4,
        cv::COLOR_RGBA2BGRA);

    set(QImage::Format_RGBX32FPx4, QImage::Format_RGBX32FPx4, CV_32FC3, cv::COLOR_RGB2BGR);
    set(QImage::Format_RGBA32FPx4, QImage::Format_RGBA32FPx4, CV_32FC4, cv::COLOR_RGBA2BGRA);
    set(QImage::Format_RGBA32FPx4_Premultiplied,
        QImage::Format_RGBA32FPx4,
        CV_32FC4,
        cv::COLOR_RGBA2BGRA);

    /* 其余格式置 Invalid，调用端可直接返回空 */
    return table;
}();

auto qImageToMat(const QImage &qimage) -> cv::Mat
{
    if (qimage.isNull()) {
        return {};
    }

    const int formatValue = qimage.format();
    if (formatValue < 0 || formatValue >= int(std::size(kQToMatTable))) {
        return {};
    }

    const auto [unifiedFormat, openCvType, openCvCode] = kQToMatTable[formatValue];
    if (unifiedFormat == QImage::Format_Invalid) {
        return {};
    }

    const auto unifiedImage = qimage.convertToFormat(unifiedFormat);

    cv::Mat rawMat(unifiedImage.height(),
                   unifiedImage.width(),
                   openCvType,
                   const_cast<uchar *>(unifiedImage.bits()),
                   unifiedImage.bytesPerLine());
    cv::Mat result = rawMat.clone(); // 脱离 Qt 内存
    if (openCvCode) {
        cv::cvtColor(result, result, openCvCode);
    }

    return result;
}

auto matToQImage(const cv::Mat &mat) -> QImage
{
    if (mat.empty()) {
        return {};
    }

    int depthIndex = -1;
    switch (mat.depth()) {
    case CV_8U: depthIndex = 0; break;
    case CV_16U: depthIndex = 1; break;
    case CV_32F: depthIndex = 2; break;
    default: depthIndex = -1; break;
    }
    const int channelCount = mat.channels();
    if (depthIndex < 0 || channelCount < 1 || channelCount > 4) {
        return {};
    }

    const auto [unifiedFormat, openCvType, openCvCode] = kMatToQTable[depthIndex][channelCount];
    if (unifiedFormat == QImage::Format_Invalid) {
        return {};
    }

    cv::Mat convertedMat;
    if (openCvCode) {
        cv::cvtColor(mat, convertedMat, openCvCode);
    } else {
        convertedMat = mat;
    }

    return QImage(convertedMat.data,
                  convertedMat.cols,
                  convertedMat.rows,
                  convertedMat.step,
                  unifiedFormat)
        .copy();
}

} // namespace OpenCVUtils

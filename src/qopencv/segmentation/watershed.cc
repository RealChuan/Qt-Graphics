#include "watershed.hpp"

#include <utils/utils.h>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

auto preprocessImage(const cv::Mat &input) -> cv::Mat
{
    cv::Mat gray, blurred, edges;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);      // 灰度化
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0); // 高斯模糊
    cv::Canny(blurred, edges, 50, 150);                 // Canny边缘检测
    return edges;
}

auto createMarkers(const cv::Mat &edges) -> cv::Mat
{
    cv::Mat markers = cv::Mat::zeros(edges.size(), CV_32S); // 创建32位有符号整数标记图像
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // 查找轮廓

    // 为每个轮廓绘制不同标签到标记图像
    for (size_t i = 0; i < contours.size(); ++i) {
        cv::drawContours(markers,
                         contours,
                         static_cast<int>(i),
                         cv::Scalar::all(static_cast<int>(i) + 1),
                         -1);
    }
    return markers;
}

auto Watershed::apply(const cv::Mat &src) -> cv::Mat
{
    return Utils::asynchronous<cv::Mat>([src]() -> cv::Mat {
        cv::Mat dst;
        try {
            auto preprocessedImage = preprocessImage(src);
            auto markers = createMarkers(preprocessedImage);
            cv::watershed(src, markers);
            cv::normalize(markers, dst, 0, 255, cv::NORM_MINMAX, CV_8U); // 归一化标记图像
            cv::applyColorMap(dst, dst, cv::COLORMAP_JET);               // 伪彩色映射
        } catch (const std::exception &e) {
            qWarning() << "Watershed segmentation failed:" << e.what();
        }
        return dst;
    });
}

} // namespace OpenCVUtils

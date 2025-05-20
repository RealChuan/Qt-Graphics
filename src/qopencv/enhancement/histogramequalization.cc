#include "histogramequalization.hpp"

#include <opencv2/imgproc.hpp>
#include <utils/utils.h>

namespace OpenCVUtils {

auto HistogramEqualization::apply(const cv::Mat &src) -> cv::Mat
{
    return Utils::asynchronous<cv::Mat>([src]() -> cv::Mat {
        cv::Mat dst;
        if (src.channels() == 3) {
            cv::Mat yuv;
            cv::cvtColor(src, yuv, cv::COLOR_BGR2YUV);
            std::vector<cv::Mat> yuv_channels;
            cv::split(yuv, yuv_channels);
            cv::equalizeHist(yuv_channels[0], yuv_channels[0]);
            cv::merge(yuv_channels, yuv);
            cv::cvtColor(yuv, dst, cv::COLOR_YUV2BGR);
        } else {
            cv::equalizeHist(src, dst);
        }
        return dst;
    });
}

} // namespace OpenCVUtils

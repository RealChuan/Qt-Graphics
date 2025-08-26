#include "histogramequalization.hpp"

#include <utils/utils.h>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

auto HistogramEqualization::apply(const cv::Mat &src) -> cv::Mat
{
    return Utils::asynchronous<cv::Mat>([src]() -> cv::Mat {
        cv::Mat dst;
        try {
            cv::Mat gray8;
            const int depth = src.depth(); // CV_8U / CV_16U / CV_32F ...
            const int ch = src.channels();

            if (ch == 1 && depth == CV_8U) {
                gray8 = src;
            } else {
                cv::Mat tmp;
                if (ch > 1) {
                    cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);
                } else {
                    tmp = src;
                }

                if (depth == CV_8U) {
                    gray8 = tmp;
                } else if (depth == CV_16U) {
                    tmp.convertTo(gray8, CV_8U, 255.0 / 65535.0);
                } else if (depth == CV_32F || depth == CV_64F) {
                    double minVal, maxVal;
                    cv::minMaxLoc(tmp, &minVal, &maxVal);
                    if (maxVal > minVal)
                        tmp.convertTo(gray8,
                                      CV_8U,
                                      255.0 / (maxVal - minVal),
                                      -255.0 * minVal / (maxVal - minVal));
                    else
                        tmp.convertTo(gray8, CV_8U, 0);
                } else {
                    cv::normalize(tmp, gray8, 0, 255, cv::NORM_MINMAX, CV_8U);
                }
            }

            cv::equalizeHist(gray8, gray8);

            if (ch == 1 && depth == CV_8U) {
                dst = gray8;
            } else {
                cv::Mat grayOriginalDepth;
                if (depth == CV_8U) {
                    grayOriginalDepth = gray8;
                } else if (depth == CV_16U) {
                    gray8.convertTo(grayOriginalDepth, CV_16U, 65535.0 / 255.0);
                } else if (depth == CV_32F || depth == CV_64F) {
                    gray8.convertTo(grayOriginalDepth, depth, 1.0 / 255.0);
                } else {
                    grayOriginalDepth = gray8.clone();
                }

                if (ch > 1) {
                    std::vector<cv::Mat> planes(ch, grayOriginalDepth);
                    cv::merge(planes, dst);
                } else {
                    dst = grayOriginalDepth;
                }
            }
        } catch (const cv::Exception &e) {
            qWarning() << "HistogramEqualization:" << e.what();
        }
        return dst;
    });
}

} // namespace OpenCVUtils

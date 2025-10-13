#include "logtransformation.hpp"

#include <utils/utils.hpp>

namespace OpenCVUtils {

cv::Mat LogTransformation::apply(const cv::Mat &src)
{
    return Utils::asynchronous<cv::Mat>([src]() -> cv::Mat {
        auto dst = src.clone();
        int rows = dst.rows;
        int cols = dst.cols;

        double c = 255 / log(1 + 255);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                for (int k = 0; k < 3; k++) {
                    double pixel = dst.at<cv::Vec3b>(i, j)[k];
                    dst.at<cv::Vec3b>(i, j)[k] = cv::saturate_cast<uchar>(c * log(1.0 + pixel));
                }
            }
        }

        return dst;
    });
}

} // namespace OpenCVUtils

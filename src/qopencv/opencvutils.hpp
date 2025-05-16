#pragma once

#include "qopencv_global.hpp"

#include <QImage>

#include <opencv2/core/mat.hpp>

namespace OpenCVUtils {

QOPENCV_EXPORT auto qImageToMat(const QImage &qimage) -> cv::Mat;

QOPENCV_EXPORT auto matToQImage(const cv::Mat &mat) -> QImage;

} // namespace OpenCVUtils

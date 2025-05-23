#pragma once

#include <qopencv/edgedetection/edgedetection.hpp>
#include <qopencv/enhancement/enhancement.hpp>
#include <qopencv/filter/filter.hpp>
#include <qopencv/segmentation/segmentation.hpp>

namespace OpenCVUtils {

QOPENCV_EXPORT auto createOpenCVOBject(Enhancement::Type type) -> OpenCVOBject *;

QOPENCV_EXPORT auto createOpenCVOBject(Filter::Type type) -> OpenCVOBject *;

QOPENCV_EXPORT auto createOpenCVOBject(EdgeDetection::Type type) -> OpenCVOBject *;

QOPENCV_EXPORT auto createOpenCVOBject(Segmentation::Type type) -> OpenCVOBject *;

} // namespace OpenCVUtils

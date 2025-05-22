#pragma once

#include <qopencv/enhancement/enhancement.hpp>
#include <qopencv/filter/filter.hpp>

namespace OpenCVUtils {

QOPENCV_EXPORT auto createOpenCVOBject(Enhancement::Type type) -> OpenCVOBject *;

QOPENCV_EXPORT auto createOpenCVOBject(Filter::Type type) -> OpenCVOBject *;

} // namespace OpenCVUtils

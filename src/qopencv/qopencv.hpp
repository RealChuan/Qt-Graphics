#pragma once

#include <qopencv/enhancement/enhancement.hpp>

namespace OpenCVUtils {

QOPENCV_EXPORT auto createOpenCVOBject(Enhancement::Type type) -> OpenCVOBject *;

}

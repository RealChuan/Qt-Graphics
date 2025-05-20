#include "qopencv.hpp"

#include <qopencv/enhancement/gaussianblur.hpp>
#include <qopencv/enhancement/histogramequalization.hpp>

namespace OpenCVUtils {

auto createOpenCVOBject(Enhancement::Type type) -> OpenCVOBject *
{
    switch (type) {
    case Enhancement::Type::HistogramEqualization: return new HistogramEqualization;
    case Enhancement::Type::GaussianBlur: return new GaussianBlur;
    default: return nullptr;
    }
    return nullptr;
}

} // namespace OpenCVUtils

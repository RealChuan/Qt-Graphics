#include "qopencv.hpp"

#include <qopencv/enhancement/bilateralfilter.hpp>
#include <qopencv/enhancement/gaussianblur.hpp>
#include <qopencv/enhancement/histogramequalization.hpp>
#include <qopencv/enhancement/medianblur.hpp>

namespace OpenCVUtils {

auto createOpenCVOBject(Enhancement::Type type) -> OpenCVOBject *
{
    switch (type) {
    case Enhancement::Type::HistogramEqualization: return new HistogramEqualization;
    case Enhancement::Type::GaussianBlur: return new GaussianBlur;
    case Enhancement::Type::BilateralFilter: return new BilateralFilter;
    case Enhancement::Type::MedianBlur: return new MedianBlur;
    default: break;
    }
    return nullptr;
}

} // namespace OpenCVUtils

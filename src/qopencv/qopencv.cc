#include "qopencv.hpp"

#include <qopencv/enhancement/bilateralfilter.hpp>
#include <qopencv/enhancement/dehazed.hpp>
#include <qopencv/enhancement/gammacorrection.hpp>
#include <qopencv/enhancement/gaussianblur.hpp>
#include <qopencv/enhancement/histogramequalization.hpp>
#include <qopencv/enhancement/linearcontrast.hpp>
#include <qopencv/enhancement/logtransformation.hpp>
#include <qopencv/enhancement/medianblur.hpp>
#include <qopencv/enhancement/sharpen.hpp>
#include <qopencv/enhancement/superresolution.hpp>

namespace OpenCVUtils {

auto createOpenCVOBject(Enhancement::Type type) -> OpenCVOBject *
{
    switch (type) {
    case Enhancement::Type::HistogramEqualization: return new HistogramEqualization;
    case Enhancement::Type::GaussianBlur: return new GaussianBlur;
    case Enhancement::Type::BilateralFilter: return new BilateralFilter;
    case Enhancement::Type::MedianBlur: return new MedianBlur;
    case Enhancement::Type::LinearContrast: return new LinearContrast;
    case Enhancement::Type::GammaCorrection: return new GammaCorrection;
    case Enhancement::Type::LogTransformation: return new LogTransformation;
    case Enhancement::Type::Sharpen: return new Sharpen;
    case Enhancement::Type::Dehazed: return new Dehazed;
    case Enhancement::Type::SuperResolution: return new SuperResolution;
    default: break;
    }
    return nullptr;
}

} // namespace OpenCVUtils

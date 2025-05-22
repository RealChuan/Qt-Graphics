#include "qopencv.hpp"

#include <qopencv/enhancement/dehazed.hpp>
#include <qopencv/enhancement/gammacorrection.hpp>
#include <qopencv/enhancement/histogramequalization.hpp>
#include <qopencv/enhancement/linearcontrast.hpp>
#include <qopencv/enhancement/logtransformation.hpp>
#include <qopencv/enhancement/sharpen.hpp>
#include <qopencv/enhancement/superresolution.hpp>
#include <qopencv/filter/bilateralfilter.hpp>
#include <qopencv/filter/gaussianblur.hpp>
#include <qopencv/filter/medianblur.hpp>

namespace OpenCVUtils {

auto createOpenCVOBject(Enhancement::Type type) -> OpenCVOBject *
{
    switch (type) {
    case Enhancement::Type::HistogramEqualization: return new HistogramEqualization;
    case Enhancement::Type::LinearContrast: return new LinearContrast;
    case Enhancement::Type::GammaCorrection: return new GammaCorrection;
    case Enhancement::Type::LogTransformation: return new LogTransformation;
    case Enhancement::Type::Sharpen: return new Sharpen;
    case Enhancement::Type::Dehazed: return new Dehazed;
#ifdef LINK_DNN_SUPERRES
    case Enhancement::Type::SuperResolution: return new SuperResolution;
#endif
    default: break;
    }
    return nullptr;
}

auto createOpenCVOBject(Filter::Type type) -> OpenCVOBject *
{
    switch (type) {
    case Filter::Type::BilateralFilter: return new BilateralFilter;
    case Filter::Type::GaussianBlur: return new GaussianBlur;
    case Filter::Type::MedianBlur: return new MedianBlur;
    default: break;
    }
    return nullptr;
}

} // namespace OpenCVUtils

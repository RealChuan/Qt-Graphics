#include "qopencv.hpp"

#include <qopencv/edgedetection/canny.hpp>
#include <qopencv/edgedetection/laplacian.hpp>
#include <qopencv/edgedetection/scharr.hpp>
#include <qopencv/edgedetection/sobel.hpp>
#include <qopencv/enhancement/dehazed.hpp>
#include <qopencv/enhancement/gammacorrection.hpp>
#include <qopencv/enhancement/histogramequalization.hpp>
#include <qopencv/enhancement/linearcontrast.hpp>
#include <qopencv/enhancement/logtransformation.hpp>
#include <qopencv/enhancement/sharpen.hpp>
#include <qopencv/enhancement/superresolution.hpp>
#include <qopencv/filter/bilateralfilter.hpp>
#include <qopencv/filter/blur.hpp>
#include <qopencv/filter/boxfilter.hpp>
#include <qopencv/filter/gaussianblur.hpp>
#include <qopencv/filter/medianblur.hpp>
#include <qopencv/segmentation/adaptivethreshold.hpp>
#include <qopencv/segmentation/threshold.hpp>

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
    case Filter::Type::Blur: return new Blur;
    case Filter::Type::BoxFilter: return new BoxFilter;
    case Filter::Type::BilateralFilter: return new BilateralFilter;
    case Filter::Type::GaussianBlur: return new GaussianBlur;
    case Filter::Type::MedianBlur: return new MedianBlur;
    default: break;
    }
    return nullptr;
}

auto createOpenCVOBject(EdgeDetection::Type type) -> OpenCVOBject *
{
    switch (type) {
    case EdgeDetection::Type::Canny: return new Canny;
    case EdgeDetection::Type::Laplacian: return new Laplacian;
    case EdgeDetection::Type::Scharr: return new Scharr;
    case EdgeDetection::Type::Sobel: return new Sobel;
    default: break;
    }
    return nullptr;
}

auto createOpenCVOBject(Segmentation::Type type) -> OpenCVOBject *
{
    switch (type) {
    case Segmentation::Type::Threshold: return new Threshold;
    case Segmentation::Type::AdaptiveThreshold: return new AdaptiveThreshold;
    default: break;
    }
    return nullptr;
}

} // namespace OpenCVUtils

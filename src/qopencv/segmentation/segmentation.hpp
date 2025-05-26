#pragma once

#include <qopencv/opencvobject.hpp>

namespace OpenCVUtils {

class QOPENCV_EXPORT Segmentation : public OpenCVOBject
{
    Q_OBJECT
public:
    enum Type : int { Threshold = 1, AdaptiveThreshold, Watershed };
    Q_ENUM(Type);

    using OpenCVOBject::OpenCVOBject;
};

} // namespace OpenCVUtils

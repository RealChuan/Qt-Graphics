#pragma once

#include <qopencv/opencvobject.hpp>

namespace OpenCVUtils {

class QOPENCV_EXPORT Filter : public OpenCVOBject
{
    Q_OBJECT
public:
    enum Type : int { Blur = 1, BoxFilter, BilateralFilter, GaussianBlur, MedianBlur };
    Q_ENUM(Type);

    using OpenCVOBject::OpenCVOBject;
};

} // namespace OpenCVUtils

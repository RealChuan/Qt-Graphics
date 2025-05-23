#pragma once

#include <qopencv/opencvobject.hpp>

namespace OpenCVUtils {

class QOPENCV_EXPORT EdgeDetection : public OpenCVOBject
{
    Q_OBJECT
public:
    enum Type : int { Canny = 1, Laplacian, Scharr, Sobel };
    Q_ENUM(Type);

    using OpenCVOBject::OpenCVOBject;
};

} // namespace OpenCVUtils

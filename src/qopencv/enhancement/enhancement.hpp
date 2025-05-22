#pragma once

#include <qopencv/opencvobject.hpp>

namespace OpenCVUtils {

class QOPENCV_EXPORT Enhancement : public OpenCVOBject
{
    Q_OBJECT
public:
    enum Type : int {
        HistogramEqualization = 0,
        LinearContrast,
        GammaCorrection,
        LogTransformation,
        Sharpen,
        Dehazed,
#ifdef LINK_DNN_SUPERRES
        SuperResolution
#endif
    };
    Q_ENUM(Type);

    using OpenCVOBject::OpenCVOBject;
};

} // namespace OpenCVUtils

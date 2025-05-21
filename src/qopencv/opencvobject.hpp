#pragma once

#include "qopencv_global.hpp"

#include <QObject>

#include <opencv2/core/mat.hpp>

class QWidget;

namespace OpenCVUtils {

class QOPENCV_EXPORT OpenCVOBject : public QObject
{
    Q_OBJECT
public:
    enum AlgorithmType : int { Enhancement = 0 };
    Q_ENUM(AlgorithmType);

    using QObject::QObject;

    virtual auto paramWidget() -> QWidget * = 0;

    virtual auto canApply() const -> bool = 0;
    virtual auto apply(const cv::Mat &src) -> cv::Mat = 0;

protected:
    virtual auto createParamWidget() -> QWidget * = 0;
};

} // namespace OpenCVUtils

#pragma once

#include "qopencv_global.hpp"

#include <QImage>
#include <QObject>

#include <opencv2/core/mat.hpp>

namespace OpenCVUtils {

QOPENCV_EXPORT auto qImageToMat(const QImage &qimage) -> cv::Mat;

QOPENCV_EXPORT auto matToQImage(const cv::Mat &mat) -> QImage;

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

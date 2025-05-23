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
    enum AlgorithmType : int { Enhancement = 0, Filter, EdgeDetection };
    Q_ENUM(AlgorithmType);

    explicit OpenCVOBject(QObject *parent = nullptr);
    virtual ~OpenCVOBject() override;

    auto paramWidget() -> QWidget *;

    virtual auto canApply() const -> bool = 0;
    virtual auto apply(const cv::Mat &src) -> cv::Mat = 0;

protected:
    virtual auto createParamWidget() -> QWidget * = 0;

    class OpenCVOBjectPrivate;
    QScopedPointer<OpenCVOBjectPrivate> d_ptr;
};

} // namespace OpenCVUtils

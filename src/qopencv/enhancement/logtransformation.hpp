#pragma once

#include "enhancement.hpp"

namespace OpenCVUtils {

class LogTransformation : public Enhancement
{
    Q_OBJECT
public:
    using Enhancement::Enhancement;

    auto canApply() const -> bool override { return true; }
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override { return nullptr; }
};

} // namespace OpenCVUtils

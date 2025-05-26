#pragma once

#include "segmentation.hpp"

namespace OpenCVUtils {

class Watershed : public Segmentation
{
    Q_OBJECT
public:
    using Segmentation::Segmentation;

    auto canApply() const -> bool override { return true; }
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override { return nullptr; }
};

} // namespace OpenCVUtils

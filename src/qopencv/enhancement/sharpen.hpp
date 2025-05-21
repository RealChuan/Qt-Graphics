#pragma once

#include "enhancement.hpp"

namespace OpenCVUtils {

class Sharpen : public Enhancement
{
    Q_OBJECT
public:
    explicit Sharpen(QObject *parent = nullptr);
    ~Sharpen() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    class SharpenPrivate;
    QScopedPointer<SharpenPrivate> d_ptr;
};

} // namespace OpenCVUtils

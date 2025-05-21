#pragma once

#include "enhancement.hpp"

namespace OpenCVUtils {

class GammaCorrection : public Enhancement
{
    Q_OBJECT
public:
    explicit GammaCorrection(QObject *parent = nullptr);
    ~GammaCorrection() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    class GammaCorrectionPrivate;
    QScopedPointer<GammaCorrectionPrivate> d_ptr;
};

} // namespace OpenCVUtils

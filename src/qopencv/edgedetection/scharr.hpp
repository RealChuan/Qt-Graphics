#pragma once

#include "edgedetection.hpp"

namespace OpenCVUtils {

class Scharr : public EdgeDetection
{
    Q_OBJECT
public:
    explicit Scharr(QObject *parent = nullptr);
    ~Scharr() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    class ScharrPrivate;
    QScopedPointer<ScharrPrivate> d_ptr;
};

} // namespace OpenCVUtils

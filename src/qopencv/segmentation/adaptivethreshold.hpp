#pragma once

#include "segmentation.hpp"

namespace OpenCVUtils {

class AdaptiveThreshold : public Segmentation
{
    Q_OBJECT
public:
    explicit AdaptiveThreshold(QObject *parent = nullptr);
    ~AdaptiveThreshold() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    void buildConnect();

    class AdaptiveThresholdPrivate;
    QScopedPointer<AdaptiveThresholdPrivate> d_ptr;
};

} // namespace OpenCVUtils

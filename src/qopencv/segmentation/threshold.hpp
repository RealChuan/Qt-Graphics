#pragma once

#include "segmentation.hpp"

namespace OpenCVUtils {

class Threshold : public Segmentation
{
    Q_OBJECT
public:
    explicit Threshold(QObject *parent = nullptr);
    ~Threshold() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    void buildConnect();

    class ThresholdPrivate;
    QScopedPointer<ThresholdPrivate> d_ptr;
};

} // namespace OpenCVUtils

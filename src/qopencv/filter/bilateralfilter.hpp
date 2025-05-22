#pragma once

#include "filter.hpp"

namespace OpenCVUtils {

class BilateralFilter : public Filter
{
    Q_OBJECT
public:
    explicit BilateralFilter(QObject *parent = nullptr);
    ~BilateralFilter() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    void buildConnect();

    class BilateralFilterPrivate;
    QScopedPointer<BilateralFilterPrivate> d_ptr;
};

} // namespace OpenCVUtils

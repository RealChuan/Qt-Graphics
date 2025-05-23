#pragma once

#include "filter.hpp"

namespace OpenCVUtils {

class BoxFilter : public Filter
{
    Q_OBJECT
public:
    explicit BoxFilter(QObject *parent = nullptr);
    ~BoxFilter() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    class BoxFilterPrivate;
    QScopedPointer<BoxFilterPrivate> d_ptr;
};

} // namespace OpenCVUtils

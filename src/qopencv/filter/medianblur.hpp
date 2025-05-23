#pragma once

#include "filter.hpp"

namespace OpenCVUtils {

class MedianBlur : public Filter
{
    Q_OBJECT
public:
    explicit MedianBlur(QObject *parent = nullptr);
    ~MedianBlur() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    class MedianBlurPrivate;
    QScopedPointer<MedianBlurPrivate> d_ptr;
};

} // namespace OpenCVUtils

#pragma once

#include "filter.hpp"

namespace OpenCVUtils {

class GaussianBlur : public Filter
{
    Q_OBJECT
public:
    explicit GaussianBlur(QObject *parent = nullptr);
    ~GaussianBlur() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    void buildConnect();

    class GaussianBlurPrivate;
    QScopedPointer<GaussianBlurPrivate> d_ptr;
};

} // namespace OpenCVUtils

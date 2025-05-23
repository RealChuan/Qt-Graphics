#pragma once

#include "filter.hpp"

namespace OpenCVUtils {

class Blur : public Filter
{
    Q_OBJECT
public:
    explicit Blur(QObject *parent = nullptr);
    ~Blur() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    void buildConnect();

    class BlurPrivate;
    QScopedPointer<BlurPrivate> d_ptr;
};

} // namespace OpenCVUtils

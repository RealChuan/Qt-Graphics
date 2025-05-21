#pragma once

#include "enhancement.hpp"

namespace OpenCVUtils {

class Dehazed : public Enhancement
{
    Q_OBJECT
public:
    explicit Dehazed(QObject *parent = nullptr);
    ~Dehazed() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    void buildConnect();

    class DehazedPrivate;
    QScopedPointer<DehazedPrivate> d_ptr;
};

} // namespace OpenCVUtils

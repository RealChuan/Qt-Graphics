#pragma once

#include "enhancement.hpp"

namespace OpenCVUtils {

class LinearContrast : public Enhancement
{
    Q_OBJECT
public:
    explicit LinearContrast(QObject *parent = nullptr);
    ~LinearContrast() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    void buildConnect();

    class LinearContrastPrivate;
    QScopedPointer<LinearContrastPrivate> d_ptr;
};

} // namespace OpenCVUtils

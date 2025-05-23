#pragma once

#include "edgedetection.hpp"

namespace OpenCVUtils {

class Laplacian : public EdgeDetection
{
    Q_OBJECT
public:
    explicit Laplacian(QObject *parent = nullptr);
    ~Laplacian() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    class LaplacianPrivate;
    QScopedPointer<LaplacianPrivate> d_ptr;
};

} // namespace OpenCVUtils

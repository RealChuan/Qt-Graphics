#pragma once

#include "edgedetection.hpp"

namespace OpenCVUtils {

class Sobel : public EdgeDetection
{
    Q_OBJECT
public:
    explicit Sobel(QObject *parent = nullptr);
    ~Sobel() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    void buildConnect();

    class SobelPrivate;
    QScopedPointer<SobelPrivate> d_ptr;
};

} // namespace OpenCVUtils

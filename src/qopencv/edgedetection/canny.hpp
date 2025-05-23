#pragma once

#include "edgedetection.hpp"

namespace OpenCVUtils {

class Canny : public EdgeDetection
{
    Q_OBJECT
public:
    explicit Canny(QObject *parent = nullptr);
    ~Canny() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

protected:
    auto createParamWidget() -> QWidget * override;

private:
    void buildConnect();

    class CannyPrivate;
    QScopedPointer<CannyPrivate> d_ptr;
};

} // namespace OpenCVUtils

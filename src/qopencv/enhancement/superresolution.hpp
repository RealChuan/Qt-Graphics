#pragma once

#include "enhancement.hpp"

namespace OpenCVUtils {

class SuperResolution : public Enhancement
{
    Q_OBJECT
public:
    explicit SuperResolution(QObject *parent = nullptr);
    ~SuperResolution() override;

    auto canApply() const -> bool override;
    auto apply(const cv::Mat &src) -> cv::Mat override;

private slots:
    void onSelectModel();

protected:
    auto createParamWidget() -> QWidget * override;

private:
    void buildConnect();

    class SuperResolutionPrivate;
    QScopedPointer<SuperResolutionPrivate> d_ptr;
};

} // namespace OpenCVUtils

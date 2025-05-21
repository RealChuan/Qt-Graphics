#pragma once

#include <qopencv/opencvobject.hpp>

namespace OpenCVUtils {

class QOPENCV_EXPORT Enhancement : public OpenCVOBject
{
    Q_OBJECT
public:
    enum Type : int { HistogramEqualization = 0, GaussianBlur, BilateralFilter, MedianBlur };
    Q_ENUM(Type);

    explicit Enhancement(QObject *parent = nullptr);
    virtual ~Enhancement();

    auto paramWidget() -> QWidget *;

private:
    class EnhancementPrivate;
    QScopedPointer<EnhancementPrivate> d_ptr;
};

} // namespace OpenCVUtils

#include "opencvobject.hpp"

#include <QtWidgets>

namespace OpenCVUtils {

class OpenCVOBject::OpenCVOBjectPrivate
{
public:
    explicit OpenCVOBjectPrivate(OpenCVOBject *q)
        : q_ptr(q)
    {}

    ~OpenCVOBjectPrivate() {}

    OpenCVOBject *q_ptr;

    QScopedPointer<QWidget> paramWidgetPtr;
};

OpenCVOBject::OpenCVOBject(QObject *parent)
    : QObject(parent)
    , d_ptr(new OpenCVOBjectPrivate(this))
{}

OpenCVOBject::~OpenCVOBject() {}

auto OpenCVOBject::paramWidget() -> QWidget *
{
    if (d_ptr->paramWidgetPtr.isNull()) {
        d_ptr->paramWidgetPtr.reset(createParamWidget());
    };
    return d_ptr->paramWidgetPtr.data();
}

} // namespace OpenCVUtils

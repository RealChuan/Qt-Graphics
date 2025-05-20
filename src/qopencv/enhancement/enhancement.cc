#include "enhancement.hpp"

#include <QtWidgets>

namespace OpenCVUtils {

class Enhancement::EnhancementPrivate
{
public:
    explicit EnhancementPrivate(Enhancement *q)
        : q_ptr(q)
    {}

    ~EnhancementPrivate() {}

    Enhancement *q_ptr;

    QScopedPointer<QWidget> paramWidgetPtr;
};

Enhancement::Enhancement(QObject *parent)
    : OpenCVOBject(parent)
    , d_ptr(new EnhancementPrivate(this))
{}

Enhancement::~Enhancement() {}

auto Enhancement::paramWidget() -> QWidget *
{
    if (d_ptr->paramWidgetPtr.isNull()) {
        d_ptr->paramWidgetPtr.reset(createParamWidget());
    };
    return d_ptr->paramWidgetPtr.data();
};

} // namespace OpenCVUtils

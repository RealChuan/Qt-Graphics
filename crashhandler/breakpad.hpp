#ifndef BREAKPAD_HPP
#define BREAKPAD_HPP

#include "crashhandler_global.h"

#include <QObject>

namespace Utils {

class CRASHHANDLER_EXPORT BreakPad : public QObject
{
public:
    explicit BreakPad(QObject *parent = nullptr);
    ~BreakPad();

private:
    struct BreakPadPrivate;
    QScopedPointer<BreakPadPrivate> d_ptr;
};

} // namespace Utils

#endif // BREAKPAD_HPP

#ifndef BREAKPAD_HPP
#define BREAKPAD_HPP

#include "thirdparty_global.hpp"

#include <QObject>

namespace Utils {

class THRIDPARTY_EXPORT BreakPad : public QObject
{
    Q_OBJECT
public:
    static BreakPad *instance();

signals:
    void crash();

private:
    explicit BreakPad(QObject *parent = nullptr);
    ~BreakPad();

    struct BreakPadPrivate;
    QScopedPointer<BreakPadPrivate> d_ptr;
};

THRIDPARTY_EXPORT void openCrashReporter();

void crash();

} // namespace Utils

#endif // BREAKPAD_HPP

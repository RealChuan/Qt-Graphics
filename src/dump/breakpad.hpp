#ifndef BREAKPAD_HPP
#define BREAKPAD_HPP

#include "dump_global.hpp"

#include <utils/singleton.hpp>

#include <QObject>

namespace Dump {

class DUMP_EXPORT BreakPad : public QObject
{
    Q_OBJECT
public:
    void setDumpPath(const QString &path);

signals:
    void crash();

private:
    explicit BreakPad(QObject *parent = nullptr);
    ~BreakPad() override;

    class BreakPadPrivate;
    QScopedPointer<BreakPadPrivate> d_ptr;

    SINGLETON(BreakPad)
};

DUMP_EXPORT void openCrashReporter();

} // namespace Dump

#endif // BREAKPAD_HPP

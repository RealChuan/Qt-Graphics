#ifndef CRASHPAD_HPP
#define CRASHPAD_HPP

#include "dump_global.hpp"

#include <QObject>

namespace Dump {

class DUMP_EXPORT CrashPad : public QObject
{
    Q_OBJECT
public:
    explicit CrashPad(const QString &dumpPath,
                      const QString &libexecPath,
                      const QString &reportUrl,
                      bool crashReportingEnabled,
                      QObject *parent = nullptr);
    ~CrashPad() override;

private:
    class CrashPadPrivate;
    QScopedPointer<CrashPadPrivate> d_ptr;
};

} // namespace Dump

#endif // CRASHPAD_HPP

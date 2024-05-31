#include "crashpad.hpp"

#include <crashpad/client/crash_report_database.h>
#include <crashpad/client/crashpad_client.h>
#include <crashpad/client/settings.h>

namespace Dump {

class CrashPad::CrashPadPrivate
{
public:
    explicit CrashPadPrivate(CrashPad *q)
        : q_ptr(q)
        , crashpadClientPtr(new crashpad::CrashpadClient)
    {}
    ~CrashPadPrivate() {}

    CrashPad *q_ptr = nullptr;

    QScopedPointer<crashpad::CrashpadClient> crashpadClientPtr;
};

CrashPad::CrashPad(const QString &dumpPath,
                   const QString &libexecPath,
                   const QString &reportUrl,
                   bool crashReportingEnabled,
                   QObject *parent)
    : QObject(parent)
    , d_ptr(new CrashPadPrivate(this))
{
    auto handlerPath = libexecPath + "/crashpad_handler";
#ifdef Q_OS_WIN
    handlerPath += ".exe";
    base::FilePath database(dumpPath.toStdWString());
    base::FilePath handler(handlerPath.toStdWString());
#else
    base::FilePath database(dumpPath.toStdString());
    base::FilePath handler(handlerPath.toStdString());
#endif
#ifdef Q_OS_LINUX
    bool asynchronous_start = false;
#else
    bool asynchronous_start = true;
#endif

    auto dbPtr = crashpad::CrashReportDatabase::Initialize(database);
    if (dbPtr && (dbPtr->GetSettings() != nullptr)) {
        dbPtr->GetSettings()->SetUploadsEnabled(crashReportingEnabled);
    }

    d_ptr->crashpadClientPtr->StartHandler(handler,
                                           database,
                                           database,
                                           reportUrl.toStdString(),
                                           {},
                                           {"--no-rate-limit"},
                                           true,
                                           asynchronous_start);
}

CrashPad::~CrashPad() {}

} // namespace Dump

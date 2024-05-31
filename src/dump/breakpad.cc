#include "breakpad.hpp"

#include <utils/utils.h>

#include <QDebug>
#include <QDesktopServices>

#if defined(Q_OS_WIN)
#include <client/windows/handler/exception_handler.h>
#elif defined(Q_OS_MAC)
#include <client/mac/handler/exception_handler.h>
#elif defined(Q_OS_LINUX)
#include <client/linux/handler/exception_handler.h>
#endif

#ifdef Q_OS_WIN
#define CrashReportName "CrashReport.exe"
#else
#define CrashReportName "CrashReport"
#endif

namespace Dump {

// 程序崩溃回调函数;
#if defined(Q_OS_WIN)
auto callback(const wchar_t *dump_path,
              const wchar_t *id,
              void * /*unused*/,
              EXCEPTION_POINTERS * /*unused*/,
              MDRawAssertionInfo * /*unused*/,
              bool succeeded) -> bool
{
    if (succeeded) {
        qInfo() << "Create dump file success:" << QString::fromWCharArray(dump_path)
                << QString::fromWCharArray(id);
        emit BreakPad::instance()->crash();
    } else {
        qWarning() << "Create dump file failed";
    }
    return succeeded;
}
#elif defined(Q_OS_MAC)
bool callback(const char *dump_dir, const char *minidump_id, void *, bool succeeded)
{
    if (succeeded) {
        qInfo() << "Create dump file success:" << dump_dir << minidump_id;
        emit BreakPad::instance()->crash();
    } else {
        qWarning() << "Create dump file failed";
    }
    return succeeded;
}
#elif defined(Q_OS_LINUX)
bool callback(const google_breakpad::MinidumpDescriptor &descriptor, void *context, bool succeeded)
{
    Q_UNUSED(context)
    if (succeeded) {
        qInfo() << "Create dump file success:" << QString::fromLocal8Bit(descriptor.path());
        emit BreakPad::instance()->crash();
    } else {
        qWarning() << "Create dump file failed";
    }
    return succeeded;
}
#endif

class BreakPad::BreakPadPrivate
{
public:
    explicit BreakPadPrivate(BreakPad *q)
        : q_ptr(q)
    {}

    void setDumpPath(const QString &path)
    {
#if defined(Q_OS_WIN)
        exceptionHandlerPtr.reset(
            new google_breakpad::ExceptionHandler(path.toStdWString(),
                                                  nullptr,
                                                  callback,
                                                  nullptr,
                                                  google_breakpad::ExceptionHandler::HANDLER_ALL));
#elif defined(Q_OS_MAC)
        exceptionHandlerPtr.reset(new google_breakpad::ExceptionHandler(path.toStdString(),
                                                                        nullptr,
                                                                        callback,
                                                                        nullptr,
                                                                        true,
                                                                        nullptr));
#elif defined(Q_OS_LINUX)
        exceptionHandlerPtr.reset(
            new google_breakpad::ExceptionHandler(google_breakpad::MinidumpDescriptor(
                                                      path.toStdString()),
                                                  nullptr,
                                                  callback,
                                                  nullptr,
                                                  true,
                                                  -1));
#endif
    }

    ~BreakPadPrivate() = default;

    BreakPad *q_ptr;
    QScopedPointer<google_breakpad::ExceptionHandler> exceptionHandlerPtr;
};

void BreakPad::setDumpPath(const QString &path)
{
    d_ptr->setDumpPath(path);
}

BreakPad::BreakPad(QObject *parent)
    : QObject{parent}
    , d_ptr(new BreakPadPrivate(this))
{}

BreakPad::~BreakPad() = default;

void openCrashReporter()
{
    const auto reporterPath = qApp->applicationDirPath() + "/" + CrashReportName;
    QStringList args{Utils::crashPath(), Utils::logPath(), qApp->applicationFilePath()};
    args.append(qApp->arguments());
    QProcess process;
    process.startDetached(reporterPath, args);
}

} // namespace Dump

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

namespace Utils {

inline QString getCrashPath()
{
    const QString path = Utils::getConfigPath() + "/crashes";
    Utils::generateDirectorys(path);
    return QDir::toNativeSeparators(path);
}

inline void openCrashDialog()
{
    QDir dir;
    const QString urlCrash = Utils::getConfigPath() + "/crashes";
    const QString urlLog = Utils::getConfigPath() + "/log";
    if (dir.exists(urlCrash)) {
        QDesktopServices::openUrl(QUrl(urlCrash, QUrl::TolerantMode));
    }
    if (dir.exists(urlLog)) {
        QDesktopServices::openUrl(QUrl(urlLog, QUrl::TolerantMode));
    }
}

// 程序崩溃回调函数;
#if defined(Q_OS_WIN)
bool callback(const wchar_t *dump_path,
              const wchar_t *id,
              void *,
              EXCEPTION_POINTERS *,
              MDRawAssertionInfo *,
              bool succeeded)
{
    if (succeeded) {
        qInfo() << "Create dump file success:" << QString::fromWCharArray(dump_path)
                << QString::fromWCharArray(id);
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
    } else {
        qWarning() << "Create dump file failed";
    }
    return succeeded;
}
#endif

struct BreakPad::BreakPadPrivate
{
    BreakPadPrivate()
    {
#if defined(Q_OS_WIN)
        exceptionHandlerPtr.reset(
            new google_breakpad::ExceptionHandler(getCrashPath().toStdWString(),
                                                  nullptr,
                                                  callback,
                                                  nullptr,
                                                  google_breakpad::ExceptionHandler::HANDLER_ALL));
#elif defined(Q_OS_MAC)
        exceptionHandlerPtr.reset(new google_breakpad::ExceptionHandler(getCrashPath().toStdString(),
                                                                        nullptr,
                                                                        callback,
                                                                        nullptr,
                                                                        true,
                                                                        nullptr));
#elif defined(Q_OS_LINUX)
        exceptionHandlerPtr.reset(
            new google_breakpad::ExceptionHandler(google_breakpad::MinidumpDescriptor(
                                                      getCrashPath().toStdString()),
                                                  nullptr,
                                                  callback,
                                                  nullptr,
                                                  true,
                                                  -1));
#endif
    }
    ~BreakPadPrivate() {}
    QScopedPointer<google_breakpad::ExceptionHandler> exceptionHandlerPtr;
};

BreakPad::BreakPad(QObject *parent)
    : QObject{parent}
    , d_ptr(new BreakPadPrivate)
{}

BreakPad::~BreakPad() {}

} // namespace Utils

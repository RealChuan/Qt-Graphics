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

namespace Utils {

QString getCrashPath()
{
    const QString path = Utils::getConfigPath() + "/crashes";
    Utils::generateDirectorys(path);
    return QDir::toNativeSeparators(path);
}

void createEnvironment()
{
    const auto strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz");
    const auto path = QString("%1/crashes/%2-System Environment.txt")
                          .arg(Utils::getConfigPath(), strTime);

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        auto systemEnviroment = QProcess::systemEnvironment();
        QString str;
        for (const QString &info : qAsConst(systemEnviroment)) {
            str += info;
            str += '\n';
        }
        file.write(str.toUtf8());
        file.flush();
        file.close();
    } else {
        qWarning() << file.errorString();
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

BreakPad *BreakPad::instance()
{
    static BreakPad breakPad;
    return &breakPad;
}

BreakPad::BreakPad(QObject *parent)
    : QObject{parent}
    , d_ptr(new BreakPadPrivate)
{}

BreakPad::~BreakPad() {}

void openCrashReporter()
{
    createEnvironment();

    const auto reporterPath = qApp->applicationDirPath() + "/" + CrashReportName;
    QStringList args{Utils::getConfigPath() + "/crashes",
                     Utils::getConfigPath() + "/log",
                     qApp->applicationFilePath()};
    args.append(qApp->arguments());
    QProcess process;
    process.startDetached(reporterPath, args);
}

void crash()
{
    int *p;
    *p = 10;
}

} // namespace Utils

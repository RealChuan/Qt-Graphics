#include "mainwindow.hpp"

#include <3rdparty/qtsingleapplication/qtsingleapplication.h>
#include <dump/breakpad.hpp>
#include <examples/appinfo.hpp>
#include <utils/logasync.h>
#include <utils/utils.h>

#include <QApplication>
#include <QDir>
#include <QNetworkProxyFactory>
#include <QStyle>

#define AppName "Qt-RhiViewer"

void setAppInfo()
{
    qApp->setApplicationVersion(AppInfo::version);
    qApp->setApplicationDisplayName(AppName);
    qApp->setApplicationName(AppName);
    qApp->setDesktopFileName(AppName);
    qApp->setOrganizationDomain(AppInfo::organizationDomain);
    qApp->setOrganizationName(AppInfo::organzationName);
    qApp->setWindowIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));
}

int main(int argc, char *argv[])
{
    Utils::setHighDpiEnvironmentVariable();

    SharedTools::QtSingleApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    SharedTools::QtSingleApplication app(AppName, argc, argv);
    if (app.isRunning()) {
        qWarning() << "This is already running";
        if (app.sendMessage("raise_window_noop", 5000)) {
            return EXIT_SUCCESS;
        }
    }

#ifndef Q_OS_WIN
    Q_INIT_RESOURCE(shader);
#endif

#ifdef Q_OS_WIN
    if (!qFuzzyCompare(app.devicePixelRatio(), 1.0)
        && QApplication::style()->objectName().startsWith(QLatin1String("windows"),
                                                          Qt::CaseInsensitive)) {
        QApplication::setStyle(QLatin1String("fusion"));
    }
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setAttribute(Qt::AA_DisableWindowContextHelpButton);
#endif
    setAppInfo();
    Dump::BreakPad::instance()->setDumpPath(Utils::crashPath());
    QDir::setCurrent(app.applicationDirPath());

    // 异步日志
    auto *log = Utils::LogAsync::instance();
    log->setLogPath(Utils::logPath());
    log->setAutoDelFile(true);
    log->setAutoDelFileDays(7);
    log->setOrientation(Utils::LogAsync::Orientation::StdAndFile);
    log->setLogLevel(QtDebugMsg);
    log->startWork();

    // Make sure we honor the system's proxy settings
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    MainWindow w;
    app.setActivationWindow(&w);
    w.show();

    auto ret = app.exec();
    log->stop();
    return ret;
}

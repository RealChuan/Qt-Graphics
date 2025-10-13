#include "mainwindow.h"

#include <3rdparty/qtsingleapplication/qtsingleapplication.h>
#include <dump/crashpad.hpp>
#include <examples/appinfo.hpp>
#include <utils/hostosinfo.h>
#include <utils/logasync.h>
#include <utils/utils.hpp>

#include <QApplication>
#include <QDir>
#include <QNetworkProxyFactory>
#include <QStyle>

#define AppName "Qt-Graphics"

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

auto main(int argc, char *argv[]) -> int
{
#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!qEnvironmentVariableIsSet("QT_OPENGL")) {
        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    }
#else
    qputenv("QSG_RHI_BACKEND", "opengl");
#endif
    Utils::setHighDpiEnvironmentVariable();
    SharedTools::QtSingleApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    SharedTools::QtSingleApplication app(AppName, argc, argv);
    if (app.isRunning()) {
        qWarning() << "This is already running";
        if (app.sendMessage("raise_window_noop", 5000)) {
            return EXIT_SUCCESS;
        }
    }
    if (Utils::HostOsInfo::isWindowsHost()) {
        // The Windows 11 default style (Qt 6.7) has major issues, therefore
        // set the previous default style: "windowsvista"
        // FIXME: check newer Qt Versions
        QApplication::setStyle(QLatin1String("windowsvista"));

        // On scaling different than 100% or 200% use the "fusion" style
        qreal tmp;
        const bool fractionalDpi = !qFuzzyIsNull(std::modf(qApp->devicePixelRatio(), &tmp));
        if (fractionalDpi) {
            QApplication::setStyle(QLatin1String("fusion"));
        }
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setAttribute(Qt::AA_DisableWindowContextHelpButton);
#endif

#ifndef Q_OS_WIN
    Q_INIT_RESOURCE(shader);
#endif

    setAppInfo();

    Dump::CrashPad crashpad(Utils::crashPath(), app.applicationDirPath(), "", true);
    // auto *breakPad = Dump::BreakPad::instance();
    // breakPad->setDumpPath(Utils::crashPath());
    // QObject::connect(breakPad, &Dump::BreakPad::crash, [] { Dump::openCrashReporter(); });

    QDir::setCurrent(app.applicationDirPath());

    auto *log = Utils::LogAsync::instance();
    log->setLogPath(Utils::logPath());
    log->setAutoDelFile(true);
    log->setAutoDelFileDays(7);
    log->setOrientation(Utils::LogAsync::Orientation::StandardAndFile);
    log->setLogLevel(QtDebugMsg);
    log->startWork();

    qInfo().noquote() << "\n\n" + Utils::systemInfo() + "\n\n";
    Utils::setPixmapCacheLimit();

    // Make sure we honor the system's proxy settings
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    MainWindow w;
    app.setActivationWindow(&w);
    w.show();

    auto ret = app.exec();
    log->stop();
    return ret;
}

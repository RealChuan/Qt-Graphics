#include <3rdparty/qtsingleapplication/qtsingleapplication.h>
#include <crashhandler/breakpad.hpp>
#include <mainwindow/mainwindow.h>
#include <utils/logasync.h>
#include <utils/utils.h>

#include <QApplication>
#include <QDir>
#include <QStyle>

void setAppInfo()
{
    qApp->setApplicationVersion("0.0.1");
    qApp->setApplicationDisplayName(QObject::tr("QGraphicsTool"));
    qApp->setApplicationName(QObject::tr("QGraphicsTool"));
    qApp->setDesktopFileName(QObject::tr("QGraphicsTool"));
    qApp->setOrganizationDomain(QObject::tr("Youth"));
    qApp->setOrganizationName(QObject::tr("Youth"));
}

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!qEnvironmentVariableIsSet("QT_OPENGL"))
        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#else
    qputenv("QSG_RHI_BACKEND", "opengl");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::Round);
#endif
    Utils::setHighDpiEnvironmentVariable();
    SharedTools::QtSingleApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    SharedTools::QtSingleApplication app("PictrueTools", argc, argv);
    if (app.isRunning()) {
        qWarning() << "This is already running";
        if (app.sendMessage("raise_window_noop", 5000)) {
            return EXIT_SUCCESS;
        }
    }

#ifdef Q_OS_WIN
    if (!qFuzzyCompare(qApp->devicePixelRatio(), 1.0)
        && QApplication::style()->objectName().startsWith(QLatin1String("windows"),
                                                          Qt::CaseInsensitive)) {
        QApplication::setStyle(QLatin1String("fusion"));
    }
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setAttribute(Qt::AA_DisableWindowContextHelpButton);
#endif

    Utils::BreakPad breakPad;
    QDir::setCurrent(app.applicationDirPath());

    // 异步日志
    Utils::LogAsync *log = Utils::LogAsync::instance();
    log->setOrientation(Utils::LogAsync::Orientation::StdAndFile);
    log->setLogLevel(QtDebugMsg);
    log->startWork();

    Utils::printBuildInfo();
    Utils::setGlobalThreadPoolMaxSize();

    setAppInfo();
    app.setWindowIcon(QIcon(":/app.ico"));

    MainWindow w;
    app.setActivationWindow(&w);
    w.show();

    int result = app.exec();
    log->stop();
    return result;
}

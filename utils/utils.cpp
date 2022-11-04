#include "utils.h"
#include "hostosinfo.h"

#include <QTextCodec>
#include <QtWidgets>

void Utils::setUTF8Code()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

void Utils::setQSS()
{
    const QJsonObject json(jsonFromFile(qApp->applicationDirPath() + "/config/config.json"));
    const QStringList qssPath(json.value("qss_files").toVariant().toStringList());
    QString qss;
    for (const QString &path : qAsConst(qssPath)) {
        qDebug() << QObject::tr("Loading QSS file: %1.").arg(path);
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << QObject::tr("Cannot open the file: %1!").arg(path) << file.errorString();
            continue;
        }
        qss.append(QLatin1String(file.readAll())).append("\n");
        file.close();
    }
    if (qss.isEmpty())
        return;
    qApp->setStyleSheet(qss);
}

void Utils::loadFonts()
{
    const QJsonObject json(jsonFromFile(qApp->applicationDirPath() + "/config/config.json"));
    const QStringList fontFiles(json.value("font_files").toVariant().toStringList());

    for (const QString &file : qAsConst(fontFiles)) {
        qDebug() << QObject::tr("Loading Font file: %1").arg(file);
        QFontDatabase::addApplicationFont(file);
    }
}

void Utils::windowCenter(QWidget *child, QWidget *parent)
{
    const QSize size = parent->size() - child->size();
    int x = qMax(0, size.width() / 2);
    int y = qMax(0, size.height() / 2);
    child->move(x, y);
}

void Utils::windowCenter(QWidget *window)
{
    const QRect rect = qApp->primaryScreen()->availableGeometry();
    int x = (rect.width() - window->width()) / 2 + rect.x();
    int y = (rect.height() - window->height()) / 2 + rect.y();
    x = qMax(0, x);
    y = qMax(0, y);
    window->move(x, y);
}

QString compilerString()
{
#if defined(__apple_build_version__) // Apple clang has other version numbers
    QString isAppleString = QLatin1String(" (Apple)");
    return QLatin1String("Clang ") + QString::number(__clang_major__) + QLatin1Char('.')
           + QString::number(__clang_minor__) + isAppleString;
#elif defined(Q_CC_GNU)
    return QLatin1String("GCC ") + QLatin1String(__VERSION__);
#elif defined(Q_CC_MSVC)
    return QString("MSVC Version: %1").arg(_MSC_VER);
#endif
    return QLatin1String("<unknown compiler>");
}

void Utils::printBuildInfo()
{
    //    qInfo() << QSysInfo::buildAbi() << QSysInfo::machineUniqueId()
    //            << QOperatingSystemVersion::current();
    const QString info = QString("Qt %1 (%2, %3 bit)")
                             .arg(qVersion(), compilerString(), QString::number(QSysInfo::WordSize));
    qInfo() << QObject::tr("Build with: ") << info;
}

void Utils::setHighDpiEnvironmentVariable()
{
    if (Utils::HostOsInfo().isMacHost())
        return;

    if (Utils::HostOsInfo().isWindowsHost() && !qEnvironmentVariableIsSet("QT_OPENGL")) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif
    }

    if (Utils::HostOsInfo().isWindowsHost()
        && !qEnvironmentVariableIsSet("QT_DEVICE_PIXEL_RATIO") // legacy in 5.6, but still functional
        && !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
        && !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
        && !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    }
}

void Utils::reboot()
{
    QProcess::startDetached(QApplication::applicationFilePath(),
                            QApplication::arguments(),
                            QDir::currentPath());
    QApplication::exit();
}

qint64 calculateDir(const QString &localPath)
{
    qint64 size = 0;
    QDir dir(localPath);
    if (!dir.exists())
        return size;
    const QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::Hidden);
    for (const QFileInfo &fi : qAsConst(entries)) {
        if (fi.isSymLink() || fi.isFile()) {
            size += fi.size();
        }
    }
    return size;
}

qint64 Utils::fileSize(const QString &localPath)
{
    qint64 size = 0;
    if (localPath.isEmpty())
        return size;

    QDirIterator it(localPath,
                    QDir::NoDotAndDotDot | QDir::Dirs | QDir::NoSymLinks | QDir::Hidden,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        size += calculateDir(it.next());
    }
    size += calculateDir(localPath);
    return size;
}

bool Utils::generateDirectorys(const QString &directory)
{
    QDir sourceDir(directory);
    if (sourceDir.exists())
        return true;

    QString tempDir;
    QStringList directorys = directory.split("/");
    for (int i = 0; i < directorys.count(); i++) {
        QString path = directorys[i];
        tempDir += path + "/";

        QDir dir(tempDir);
        if (!dir.exists() && !dir.mkdir(tempDir))
            return false;
    }

    return true;
}

void removeFiles(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
        return;
    const QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::Hidden);
    for (const QFileInfo &fi : qAsConst(entries)) {
        if (fi.isSymLink() || fi.isFile()) {
            QFile f(fi.filePath());
            if (!f.remove()) {
                const QString errorMessage = QObject::tr("Cannot remove file \"%1\": %2")
                                                 .arg(QDir::toNativeSeparators(f.fileName()),
                                                      f.errorString());
                qWarning() << errorMessage;
            }
        }
    }
}

static QString errnoToQString(int error)
{
#if defined(Q_OS_WIN) && !defined(Q_CC_MINGW)
    char msg[128];
    if (strerror_s(msg, sizeof msg, error) != 0)
        return QString::fromLocal8Bit(msg);
    return QString();
#else
    return QString::fromLocal8Bit(strerror(error));
#endif
}

void Utils::removeDirectory(const QString &path)
{
    if (path.isEmpty()) // QDir("") points to the working directory! We never want to remove that one.
        return;

    QStringList dirs;
    QDirIterator it(path,
                    QDir::NoDotAndDotDot | QDir::Dirs | QDir::NoSymLinks | QDir::Hidden,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        dirs.prepend(it.next());
        removeFiles(dirs.at(0));
    }

    QDir d;
    dirs.append(path);
    removeFiles(path);
    for (const QString &dir : qAsConst(dirs)) {
        errno = 0;
        if (d.exists(path) && !d.rmdir(dir)) {
            const QString errorMessage = QObject::tr("Cannot remove directory \"%1\": %2")
                                             .arg(QDir::toNativeSeparators(dir),
                                                  errnoToQString(errno));
            qWarning() << errorMessage;
        }
    }
}

QString Utils::bytesToString(qint64 bytes)
{
    const double KB = 1024 * 1.0;
    const double MB = KB * 1024;
    const double GB = MB * 1024;
    const double TB = GB * 1024;

    if (bytes / TB >= 1)
        return QString("%1TB").arg(QString::number(bytes / TB, 'f', 2));
    else if (bytes / GB >= 1)
        return QString("%1GB").arg(QString::number(bytes / GB, 'f', 2));
    else if (bytes / MB >= 1)
        return QString("%1MB").arg(QString::number(bytes / MB, 'f', 2));
    else if (bytes / KB >= 1)
        return QString("%1KB").arg(qint64(bytes / KB));
    else
        return QString("%1B").arg(bytes);
}

QJsonObject Utils::jsonFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << QObject::tr("Cannot open the file: %1").arg(filePath);
        return QJsonObject();
    }
    const QByteArray buf(file.readAll());
    file.close();
    return jsonFromBytes(buf);
}

QJsonObject Utils::jsonFromBytes(const QByteArray &bytes)
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(bytes, &jsonParseError);
    if (QJsonParseError::NoError != jsonParseError.error) {
        qWarning() << QObject::tr("%1\nOffset: %2")
                          .arg(jsonParseError.errorString(), jsonParseError.offset);
        return QJsonObject();
    }
    return jsonDocument.object();
}

void Utils::setGlobalThreadPoolMaxSize(int maxSize)
{
    QThreadPool *instance = QThreadPool::globalInstance();
    if (maxSize > 0) {
        instance->setMaxThreadCount(maxSize);
        return;
    }
    instance->setMaxThreadCount(qMax(4, 2 * instance->maxThreadCount()));
}

QString Utils::getConfigPath()
{
    static QString path;
    if (path.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        if (path.isEmpty()) {
            path = QDir::homePath();
        }
        if (!path.endsWith(qAppName())) {
            path = path + "/" + qAppName();
        }
    }
    //qInfo() << path;
    Utils::generateDirectorys(path);
    return path;
}

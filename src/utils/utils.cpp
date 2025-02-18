#include "utils.h"
#include "hostosinfo.h"
#include "utilstr.h"

#include <QTextCodec>
#include <QtWidgets>

// clang-format off
#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>
#endif
// clang-format on

auto Utils::desktopGeometry() -> QRect
{
    QRect geometry;
    auto screens = QGuiApplication::screens();
    for (auto *const screen : std::as_const(screens)) {
        QRect scrRect = screen->geometry();
        scrRect.moveTo(scrRect.x() / screen->devicePixelRatio(),
                       scrRect.y() / screen->devicePixelRatio());
        geometry = geometry.united(scrRect);
    }
    return geometry;
}

auto Utils::grabFullWindow() -> QPixmap
{
    // auto pixmap = screen()->grabWindow();
    // auto pixmap = QApplication::primaryScreen()->grabWindow();
    // fix me
    // same code in Qt5高分屏没问题，
    // Qt6图片大小是缩放倍数，多余部分是黑色的
    auto geometry = desktopGeometry();
    auto pixmap = QApplication::primaryScreen()->grabWindow(0,
                                                            geometry.x(),
                                                            geometry.y(),
                                                            geometry.width(),
                                                            geometry.height());
    //pixmap.setDevicePixelRatio(QApplication::primaryScreen()->devicePixelRatio());
    return pixmap;
}

void Utils::setUTF8Code()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

void Utils::setQSS(const QStringList &qssFilePaths)
{
    QStringList qssFiles;
    for (const auto &path : std::as_const(qssFilePaths)) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << QString("Cannot open the file: %1").arg(path) << file.errorString();
            continue;
        }
        qInfo() << QString("Load QSS: %1").arg(path);
        qssFiles.append(QLatin1String(file.readAll()));
        file.close();
    }
    if (qssFiles.isEmpty()) {
        return;
    }
    qApp->setStyleSheet(qssFiles.join("\n"));
}

void Utils::loadFonts(const QString &fontPath)
{
    const QDir dir(fontPath);
    if (!dir.exists()) {
        return;
    }
    // QFontDatabase::removeAllApplicationFonts();
    const auto fonts = dir.entryInfoList(QStringList("*.ttf"), QDir::Files);
    for (const auto &fileInfo : std::as_const(fonts)) {
        int fontId = QFontDatabase::addApplicationFont(fileInfo.absoluteFilePath());
        if (fontId == -1) {
            qWarning() << QString("Loading Fonts file: %1 Failed.").arg(fileInfo.fileName());
        } else {
            qInfo() << QString("Loading Fonts file: %1.").arg(fileInfo.fileName())
                    << QFontDatabase::applicationFontFamilies(fontId);
        }
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

auto compilerString() -> QString
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

auto Utils::isMainThread() -> bool
{
    return QThread::currentThread() == qApp->thread();
}

auto Utils::systemInfo() -> QString
{
    auto text = QString("%1 (%2) on %3 (%4) with CPU Cores: %5")
                    .arg(QSysInfo::prettyProductName(),
                         QSysInfo::kernelVersion(),
                         QSysInfo::currentCpuArchitecture(),
                         QSysInfo::machineHostName(),
                         QString::number(QThread::idealThreadCount()))
                + "\n"
                + QString("Build with: Qt %1 (%2, %3)")
                      .arg(qVersion(), compilerString(), QSysInfo::buildAbi());
    return text;
}

void Utils::setHighDpiEnvironmentVariable()
{
#ifdef Q_OS_WIN

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!qEnvironmentVariableIsSet("QT_DEVICE_PIXEL_RATIO") // legacy in 5.6, but still functional
        && !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
        && !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
        && !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    }
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

#endif // Q_OS_WIN
}

void Utils::reboot()
{
    QProcess::startDetached(QApplication::applicationFilePath(),
                            QApplication::arguments(),
                            QDir::currentPath());
    QApplication::exit();
}

auto Utils::createDirectoryRecursively(const QString &path) -> bool
{
    return QDir().mkpath(path);
}

auto Utils::removeDirectory(const QString &path) -> bool
{
    if (path.isEmpty()) { // QDir("") points to the working directory! We never want to remove that one.
        return false;
    }

    QDir dir(path);
    if (!dir.exists()) {
        qWarning() << Tr::tr("Directory does not exist: %1").arg(path);
        return false;
    }
    return dir.removeRecursively();
}

auto Utils::removeFile(const QString &path) -> bool
{
    if (path.isEmpty()) {
        return false;
    }
    QFile file(path);
    if (!file.exists()) {
        qWarning() << Tr::tr("File does not exist: %1").arg(path);
        return false;
    }
    return file.remove();
}

QString Utils::formatBytes(qint64 bytes, int precision)
{
    const QStringList list = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    const int unit = 1024;
    int index = 0;
    double size = bytes;
    while (size >= unit) {
        size /= unit;
        index++;
    }
    Q_ASSERT_X(index < list.size(), "formatBytes", "size is too large");
    return QString("%1 %2").arg(QString::number(size, 'f', precision)).arg(list.at(index));
}

auto Utils::jsonFromFile(const QString &filePath) -> QJsonObject
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << Tr::tr("Cannot open the file: %1").arg(filePath);
        return {};
    }
    const QByteArray buf(file.readAll());
    file.close();
    return jsonFromBytes(buf);
}

auto Utils::jsonFromBytes(const QByteArray &bytes) -> QJsonObject
{
    QJsonParseError jsonParseError;
    auto jsonDocument = QJsonDocument::fromJson(bytes, &jsonParseError);
    if (QJsonParseError::NoError != jsonParseError.error) {
        qWarning() << Tr::tr("%1\nOffset: %2")
                          .arg(jsonParseError.errorString(), jsonParseError.offset);
        return {};
    }
    return jsonDocument.object();
}

auto Utils::configLocation() -> QString
{
    static QString path;
    if (path.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        if (path.isEmpty()) {
            path = QDir::homePath();
        }
    }
    //qInfo() << path;
    Utils::createDirectoryRecursively(path);
    return path;
}

auto Utils::execMenuAtWidget(QMenu *menu, QWidget *widget) -> QAction *
{
    QPoint p;
    QRect screen = widget->screen()->availableGeometry();
    QSize sh = menu->sizeHint();
    QRect rect = widget->rect();
    if (widget->isRightToLeft()) {
        if (widget->mapToGlobal(QPoint(0, rect.bottom())).y() + sh.height() <= screen.height()) {
            p = widget->mapToGlobal(rect.bottomRight());
        } else {
            p = widget->mapToGlobal(rect.topRight() - QPoint(0, sh.height()));
        }
        p.rx() -= sh.width();
    } else {
        if (widget->mapToGlobal(QPoint(0, rect.bottom())).y() + sh.height() <= screen.height()) {
            p = widget->mapToGlobal(rect.bottomLeft());
        } else {
            p = widget->mapToGlobal(rect.topLeft() - QPoint(0, sh.height()));
        }
    }
    p.rx() = qMax(screen.left(), qMin(p.x(), screen.right() - sh.width()));
    p.ry() += 1;

    return menu->exec(p);
}

auto Utils::getPidFromProcessName(const QString &processName) -> qint64
{
#if defined(Q_OS_WIN)
    auto *hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return -1;
    }
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        return -1;
    }
    do {
        if (QString::fromWCharArray(pe.szExeFile) == processName) {
            CloseHandle(hSnapshot);
            return pe.th32ProcessID;
        }
    } while (Process32Next(hSnapshot, &pe));
    CloseHandle(hSnapshot);
    return -1;
#else
    // 创建一个QProcess对象
    QProcess process;
    // 启动ps -A命令
    process.start("ps -A");
    // 等待命令执行完成
    process.waitForFinished();
    // 读取命令输出并转换为QString
    auto output = QString::fromLocal8Bit(process.readAllStandardOutput());
    // 按行分割输出
    auto lines = output.split("\n");
    // 创建一个正则表达式对象，匹配以空格+processName结尾的字符串
    QRegularExpression re("\\s" + processName + "$");
    // 遍历输出的每一行
    for (const auto &line : lines) {
        // 如果匹配成功
        if (re.match(line).hasMatch()) {
            // 按空格分割字符串
            auto parts = line.split(" ");
            // 返回pid
            return parts.at(0).toLongLong();
        }
    }
    return -1;
#endif
}

auto Utils::killProcess(qint64 pid) -> bool
{
    qWarning() << "kill process: " << pid;
#if defined(Q_OS_WIN)
    auto *hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == nullptr) {
        return false;
    }
    auto result = TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
    return result != 0;
#else
    return QProcess::execute("kill", QStringList() << QString::number(pid)) == 0;
#endif
}

void Utils::setMacComboBoxStyle(QWidget *parent)
{
#ifndef Q_OS_MACOS
    return;
#endif
    auto comboBoxs = parent->findChildren<QComboBox *>();
    for (auto *const comboBox : comboBoxs) {
        comboBox->setStyle(QStyleFactory::create("Fusion"));
    }
}

void Utils::quitApplication()
{
    QMetaObject::invokeMethod(qApp, &QApplication::quit, Qt::QueuedConnection);
}

auto Utils::crashPath() -> QString
{
    const auto path = configLocation() + "/crash";
    createDirectoryRecursively(path);
    return path;
}

auto Utils::logPath() -> QString
{
    const auto path = configLocation() + "/log";
    createDirectoryRecursively(path);
    return path;
}

auto Utils::configPath() -> QString
{
    const auto path = configLocation() + "/config";
    createDirectoryRecursively(path);
    return path;
}

auto Utils::configFilePath() -> QString
{
    return (configPath() + "/config.ini");
}

auto Utils::cachePath() -> QString
{
    const auto path = configLocation() + "/cache";
    createDirectoryRecursively(path);
    return path;
}

auto Utils::calculateDirectoryStats(const QString &path) -> DirectoryStats
{
    DirectoryStats stats;
    if (path.isEmpty() || !QDir(path).exists()) {
        return stats;
    }

    QStack<QString> dirs;
    dirs.push(path);

    while (!dirs.isEmpty()) {
        QString dirPath = dirs.pop();
        QDirIterator it(dirPath, QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);

        while (it.hasNext()) {
            const QFileInfo info(it.next());
            if (info.isDir()) {
                dirs.push(info.filePath());
                stats.folders++;
            } else {
                stats.size += info.size();
                stats.files++;
            }
        }
    }

    return stats;
}

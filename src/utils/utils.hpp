#pragma once

#include "utils_global.h"

#include <QImage>
#include <QJsonObject>
#include <QtConcurrent>
#include <QtCore>

class QWidget;
class QAction;
class QMenu;

namespace Utils {

template<typename T>
auto asynchronous(std::function<T()> &&func)
{
    if (nullptr == func) {
        return T{};
    }
    QEventLoop loop;
    QFutureWatcher<T> watcher;
    QObject::connect(&watcher, &QFutureWatcher<T>::finished, &loop, &QEventLoop::quit);
    watcher.setFuture(QtConcurrent::run([func = std::move(func)]() mutable { return func(); }));
    loop.exec();

    return watcher.result();
}

using Images = QList<QImage>;

UTILS_EXPORT Images readImages(const QString &path);
UTILS_EXPORT QWidget *getTopParentWidget(QWidget *child);
UTILS_EXPORT QRect desktopGeometry();
UTILS_EXPORT QPixmap grabFullWindow();

struct UTILS_EXPORT DirectoryStats
{
    qint64 folders = 0;
    qint64 files = 0;
    qint64 size = 0;
};
UTILS_EXPORT auto calculateDirectoryStats(const QString &path) -> DirectoryStats;

UTILS_EXPORT auto configLocation() -> QString;
UTILS_EXPORT auto configPath() -> QString;
UTILS_EXPORT auto configFilePath() -> QString;
UTILS_EXPORT auto crashPath() -> QString;
UTILS_EXPORT auto logPath() -> QString;
UTILS_EXPORT auto cachePath() -> QString;

UTILS_EXPORT void restoreAndActivate(QWidget *window);
UTILS_EXPORT void addGraphicsDropShadowEffect(QWidget *widget, int blurRadius = 10);
UTILS_EXPORT auto systemInfo() -> QString;
UTILS_EXPORT void setHighDpiEnvironmentVariable();
UTILS_EXPORT void quitApplication();
UTILS_EXPORT void setUTF8Code();
UTILS_EXPORT void setQSS(const QStringList &qssFilePaths);
UTILS_EXPORT void setPixmapCacheLimit();
UTILS_EXPORT void loadFonts(const QString &fontPath);
UTILS_EXPORT void windowCenter(QWidget *child, QWidget *parent);
UTILS_EXPORT void windowCenter(QWidget *window);
UTILS_EXPORT void reboot();
UTILS_EXPORT auto createDirectoryRecursively(const QString &path) -> bool;
UTILS_EXPORT auto removeDirectory(const QString &path) -> bool;
UTILS_EXPORT auto removeFile(const QString &path) -> bool;
UTILS_EXPORT auto formatBytes(qint64 bytes, int precision = 2) -> QString;
UTILS_EXPORT auto jsonFromFile(const QString &filePath) -> QJsonObject;
UTILS_EXPORT auto jsonFromBytes(const QByteArray &bytes) -> QJsonObject;
UTILS_EXPORT void setMacComboBoxStyle(QWidget *parent);
UTILS_EXPORT auto getPidFromProcessName(const QString &processName) -> qint64;
UTILS_EXPORT auto killProcess(qint64 pid) -> bool;
UTILS_EXPORT auto cpuBench(int iterations,
                           int durationMs,
                           int dataSize = 1024 * 1024,
                           QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha256)
    -> double;

} // namespace Utils

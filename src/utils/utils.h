#ifndef UTILS_H
#define UTILS_H

#include "utils_global.h"

#include <QJsonObject>
#include <QtCore>

class QWidget;
class QAction;
class QMenu;

namespace Utils {

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

UTILS_EXPORT auto systemInfo() -> QString;
UTILS_EXPORT void setHighDpiEnvironmentVariable();
UTILS_EXPORT void quitApplication();
UTILS_EXPORT void setUTF8Code();
UTILS_EXPORT void setQSS(const QStringList &qssFilePaths);
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
UTILS_EXPORT auto execMenuAtWidget(QMenu *menu, QWidget *widget) -> QAction *;
UTILS_EXPORT void setMacComboBoxStyle(QWidget *parent);
UTILS_EXPORT auto getPidFromProcessName(const QString &processName) -> qint64;
UTILS_EXPORT auto killProcess(qint64 pid) -> bool;

} // namespace Utils

#endif // UTILS_H

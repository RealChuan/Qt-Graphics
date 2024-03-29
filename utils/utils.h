#ifndef UTILS_H
#define UTILS_H

#include "utils_global.h"

#include <QJsonObject>
#include <QtCore>

class QWidget;

namespace Utils {

UTILS_EXPORT void printBuildInfo();
UTILS_EXPORT void setHighDpiEnvironmentVariable();
UTILS_EXPORT void setUTF8Code();
UTILS_EXPORT void setQSS();
UTILS_EXPORT void loadFonts();
UTILS_EXPORT void setGlobalThreadPoolMaxSize(int maxSize = -1);
UTILS_EXPORT void windowCenter(QWidget *child, QWidget *parent);
UTILS_EXPORT void windowCenter(QWidget *window);
UTILS_EXPORT QRect desktopGeometry();
UTILS_EXPORT QPixmap grabFullWindow();
UTILS_EXPORT void reboot();
UTILS_EXPORT auto fileSize(const QString &localPath) -> qint64;
UTILS_EXPORT auto generateDirectorys(const QString &directory) -> bool;
UTILS_EXPORT void removeDirectory(const QString &path);
UTILS_EXPORT auto convertBytesToString(qint64 bytes) -> QString;
UTILS_EXPORT QJsonObject jsonFromFile(const QString &filePath);
UTILS_EXPORT QJsonObject jsonFromBytes(const QByteArray &bytes);
UTILS_EXPORT auto getConfigPath() -> QString;

} // namespace Utils

#endif // UTILS_H

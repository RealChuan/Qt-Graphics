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
UTILS_EXPORT void reboot();
UTILS_EXPORT qint64 fileSize(const QString &localPath);
UTILS_EXPORT bool generateDirectorys(const QString &directory);
UTILS_EXPORT void removeDirectory(const QString &path);
UTILS_EXPORT QString bytesToString(qint64 bytes);
UTILS_EXPORT QJsonObject jsonFromFile(const QString &filePath);
UTILS_EXPORT QJsonObject jsonFromBytes(const QByteArray &bytes);
UTILS_EXPORT QString getConfigPath();

} // namespace Utils

#endif // UTILS_H

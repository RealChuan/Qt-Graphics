#ifndef LOGASYNC_H
#define LOGASYNC_H

#include <QThread>

#include "singleton.hpp"
#include "utils_global.h"

namespace Utils {

struct FileUtilPrivate;
class FileUtil : public QObject
{
    Q_OBJECT
public:
    explicit FileUtil(qint64 days = 7, QObject *parent = nullptr);
    ~FileUtil();

public slots:
    void onWrite(const QString &);

private slots:
    void onFlush();

private:
    QString getFileName(qint64 *now) const;
    bool rollFile(int);
    void autoDelFile();
    void setTimer();

    QScopedPointer<FileUtilPrivate> d_ptr;
};

struct LogAsyncPrivate;
class UTILS_EXPORT LogAsync : public QThread
{
    Q_OBJECT
public:
    enum Orientation { Std = 1, File, StdAndFile };

    void setOrientation(Orientation);
    Orientation orientation();

    void setLogLevel(QtMsgType);
    QtMsgType logLevel();

    void startWork();
    void stop();

signals:
    void appendBuf(const QString &);

protected:
    void run() override;

private:
    explicit LogAsync(QObject *parent = nullptr);
    ~LogAsync() override;

    QScopedPointer<LogAsyncPrivate> d_ptr;

    Singleton(LogAsync)
};

} // namespace Utils

#endif // LOGASYNC_H

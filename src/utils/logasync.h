#pragma once

#include <QThread>

#include "singleton.hpp"
#include "utils_global.h"

namespace Utils {

class UTILS_EXPORT LogAsync : public QThread
{
    Q_OBJECT
public:
    enum Orientation : int { Std = 1, File, StdAndFile };

    void setLogPath(const QString &path);
    auto logPath() -> QString;

    void setAutoDelFile(bool on);
    auto autoDelFile() -> bool;

    void setAutoDelFileDays(qint64 days);
    auto autoDelFileDays() -> qint64;

    void setOrientation(Orientation orientation);
    auto orientation() -> Orientation;

    void setLogLevel(QtMsgType type);
    auto logLevel() -> QtMsgType;

    void setMaxConsoleLineSize(int size);
    auto maxConsoleLineSize() -> int;

    void startWork();
    void stop();

signals:
    void appendBuf(const QString &);

protected:
    void run() override;

private:
    explicit LogAsync(QObject *parent = nullptr);
    ~LogAsync() override;

    class LogAsyncPrivate;
    QScopedPointer<LogAsyncPrivate> d_ptr;

    SINGLETON(LogAsync)
};

} // namespace Utils

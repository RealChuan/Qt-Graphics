#pragma once

#include <QObject>

namespace Utils {

class LogFile : public QObject
{
    Q_OBJECT
public:
    explicit LogFile(QObject *parent = nullptr);
    ~LogFile() override;

public slots:
    void onWrite(const QString &msg);

private slots:
    void onFlush();

private:
    auto rollFile(int count) -> bool;
    void setTimer();

    class LogFilePrivate;
    QScopedPointer<LogFilePrivate> d_ptr;
};

} // namespace Utils

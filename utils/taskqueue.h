#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <QMutex>
#include <QQueue>

namespace Utils {

/*----------------------------------------------------------------------------/
/                            线程安全的有界队列 模板                           /
/----------------------------------------------------------------------------*/

template<typename T>
class Queue
{
public:
    Queue()
        : m_T()
        , m_mutex()
        , m_maxSize(100)
    {}
    ~Queue() {}

    auto enqueue(const T &t) -> bool
    {
        QMutexLocker locker(&m_mutex);
        if (m_T.size() >= m_maxSize)
            return false;
        m_T.enqueue(t);
        return true;
    }

    auto enqueue(T &&t) -> bool
    {
        QMutexLocker locker(&m_mutex);
        if (m_T.size() >= m_maxSize)
            return false;
        m_T.enqueue(t);
        return true;
    }

    auto dequeue() -> T
    {
        QMutexLocker locker(&m_mutex);
        if (!m_T.isEmpty())
            return m_T.dequeue();
        return T();
    }

    auto isEmpty() -> bool
    {
        QMutexLocker locker(&m_mutex);
        if (m_T.isEmpty())
            return true;
        return false;
    }

    void clearPoints()
    {
        QMutexLocker locker(&m_mutex);
        if (m_T.isEmpty())
            return;
        qDeleteAll(m_T);
        m_T.clear();
    }

    void clear() { m_T.clear(); }

    [[nodiscard]] auto size() const -> int
    {
        QMutexLocker locker(&m_mutex);
        return m_T.size();
    }

    void setMaxSize(const int maxSize)
    {
        QMutexLocker locker(&m_mutex);
        m_maxSize = maxSize;
    }

private:
    Q_DISABLE_COPY(Queue)

    QQueue<T> m_T;
    mutable QMutex m_mutex;
    int m_maxSize;
};

} // namespace Utils

#endif // TASKQUEUE_H

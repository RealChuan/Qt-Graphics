#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <QMutex>

namespace Utils {

template<typename T>
class Singleton
{
    Q_DISABLE_COPY_MOVE(Singleton)
public:
    static T *getInstance();

private:
    Singleton() = default;
    ~Singleton() = default;

    static QMutex m_mutex;
};

template<typename T>
QMutex Singleton<T>::m_mutex;
template<typename T>
T *Singleton<T>::getInstance()
{
    QMutexLocker locker(&m_mutex);
    static T t;
    return &t;
}

} // namespace Utils

#define Singleton(Class) \
private: \
    Q_DISABLE_COPY_MOVE(Class); \
    friend class Utils::Singleton<Class>; \
\
public: \
    static Class *instance() { return Utils::Singleton<Class>::getInstance(); }

#endif // SINGLETON_HPP

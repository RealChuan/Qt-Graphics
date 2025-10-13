#pragma once

#include <QObject>

namespace Utils {

template<typename T>
class Singleton
{
    Q_DISABLE_COPY_MOVE(Singleton)
public:
    static auto getInstance() -> T *;

private:
    Singleton() = default;
    ~Singleton() = default;
};

template<typename T>
auto Singleton<T>::getInstance() -> T *
{
    static T t;
    return &t;
}

} // namespace Utils

#define SINGLETON(Class) \
private: \
    Q_DISABLE_COPY_MOVE(Class) \
    friend class Utils::Singleton<Class>; \
\
public: \
    static Class *instance() \
    { \
        return Utils::Singleton<Class>::getInstance(); \
    }

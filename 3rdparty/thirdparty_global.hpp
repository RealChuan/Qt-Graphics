#ifndef THIRDPARTY_GLOBAL_HPP
#define THIRDPARTY_GLOBAL_HPP

#include <QtCore/qglobal.h>

#if defined(THRIDPARTY_LIBRARY)
#define THRIDPARTY_EXPORT Q_DECL_EXPORT
#elif  defined(THRIDPARTY_STATIC_LIBRARY) // Abuse single files for manual tests
#  define THRIDPARTY_EXPORT
#else
#define THRIDPARTY_EXPORT Q_DECL_IMPORT
#endif

#endif // THIRDPARTY_GLOBAL_HPP

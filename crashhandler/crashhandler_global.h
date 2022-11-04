#ifndef CRASHHANDLER_GLOBAL_H
#define CRASHHANDLER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CRASHHANDLER_LIBRARY)
#  define CRASHHANDLER_EXPORT Q_DECL_EXPORT
#elif  defined(CRASHHANDLER_STATIC_LIBRARY) // Abuse single files for manual tests
#  define CRASHHANDLER_EXPORT
#else
#  define CRASHHANDLER_EXPORT Q_DECL_IMPORT
#endif

#endif // CRASHHANDLER_GLOBAL_H

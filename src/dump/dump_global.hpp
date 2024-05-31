#ifndef DUMP_GLOBAL_HPP
#define DUMP_GLOBAL_HPP

#include <QtCore/qglobal.h>

#if defined(DUMP_LIBRARY)
#define DUMP_EXPORT Q_DECL_EXPORT
#else
#define DUMP_EXPORT Q_DECL_IMPORT
#endif

#endif // DUMP_GLOBAL_HPP

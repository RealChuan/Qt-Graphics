#ifndef GRAPHICS_GLOBAL_H
#define GRAPHICS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GRAPHICS_LIBRARY)
#  define GRAPHICS_EXPORT Q_DECL_EXPORT
#else
#  define GRAPHICS_EXPORT Q_DECL_IMPORT
#endif

#endif // GRAPHICS_GLOBAL_H

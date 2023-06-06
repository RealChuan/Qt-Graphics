#pragma once

#include <QtCore/qglobal.h>

#if defined(OPENGLGRAPHICS_LIBRARY)
#define OPENGLGRAPHICS Q_DECL_EXPORT
#else
#define OPENGLGRAPHICS Q_DECL_IMPORT
#endif

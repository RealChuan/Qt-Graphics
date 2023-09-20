#pragma once

#include <QtCore/qglobal.h>

#if defined(GPUGRAPHICS_LIBRARY)
#define GPUAPHICS Q_DECL_EXPORT
#else
#define GPUAPHICS Q_DECL_IMPORT
#endif

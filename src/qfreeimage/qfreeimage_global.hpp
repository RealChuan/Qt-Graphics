#pragma once

#include <QtCore/qglobal.h>

#if defined(QFREEIMAGE_LIBRARY)
#define QFREEIMAGE_EXPORT Q_DECL_EXPORT
#else
#define QFREEIMAGE_EXPORT Q_DECL_IMPORT
#endif

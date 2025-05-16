#pragma once

#include <QtCore/qglobal.h>

#if defined(QOPENCV_LIBRARY)
#define QOPENCV_EXPORT Q_DECL_EXPORT
#else
#define QOPENCV_EXPORT Q_DECL_IMPORT
#endif

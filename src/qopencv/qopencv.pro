include(../3rdparty/3rdparty.pri)
include(../slib.pri)

QT += widgets concurrent

DEFINES += QOPENCV_LIBRARY
TARGET = $$replaceLibName(qopencv)

include(enhancement/enhancement.pri)

HEADERS += \
    opencvutils.hpp \
    qopencv.hpp \
    qopencv_global.hpp

SOURCES += \
    opencvutils.cc \
    qopencv.cc

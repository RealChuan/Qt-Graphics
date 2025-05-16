include(../3rdparty/3rdparty.pri)
include(../slib.pri)

QT += gui

DEFINES += QOPENCV_LIBRARY
TARGET = $$replaceLibName(qopencv)

HEADERS += \
    opencvutils.hpp \
    qopencv_global.hpp

SOURCES += \
    opencvutils.cc

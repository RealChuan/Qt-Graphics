include(../3rdparty/3rdparty.pri)
include(../slib.pri)

QT += widgets concurrent

DEFINES += QOPENCV_LIBRARY
TARGET = $$replaceLibName(qopencv)

LIBS += -l$$replaceLibName(utils)

include(enhancement/enhancement.pri)
include(filter/filter.pri)

HEADERS += \
    opencvobject.hpp \
    opencvutils.hpp \
    qopencv.hpp \
    qopencv_global.hpp

SOURCES += \
    opencvobject.cc \
    opencvutils.cc \
    qopencv.cc

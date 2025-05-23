include(../3rdparty/3rdparty.pri)
include(../slib.pri)

QT += widgets concurrent

DEFINES += QOPENCV_LIBRARY
TARGET = $$replaceLibName(qopencv)

LIBS += -l$$replaceLibName(utils)

include(enhancement/enhancement.pri)
include(filter/filter.pri)
include(edgedetection/edgedetection.pri)
include(segmentation/segmentation.pri)

HEADERS += \
    opencvobject.hpp \
    opencvutils.hpp \
    qopencv.hpp \
    qopencv_global.hpp

SOURCES += \
    opencvobject.cc \
    opencvutils.cc \
    qopencv.cc

include(../../qmake/PlatformLibraries.pri)
include(enhancement/enhancement.pri)
include(filter/filter.pri)
include(edgedetection/edgedetection.pri)
include(segmentation/segmentation.pri)

QT += widgets concurrent

DEFINES += QOPENCV_LIBRARY
TARGET = $$add_platform_library(qopencv)

LIBS += -l$$replaceLibName(utils)

include(../../qmake/VcpkgToolchain.pri)

HEADERS += \
    opencvobject.hpp \
    opencvutils.hpp \
    qopencv.hpp \
    qopencv_global.hpp

SOURCES += \
    opencvobject.cc \
    opencvutils.cc \
    qopencv.cc

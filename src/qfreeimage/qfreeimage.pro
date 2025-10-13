include(../../qmake/PlatformLibraries.pri)

QT += widgets concurrent

DEFINES += QGRAPHICSMAGICK_LIBRARY
TARGET = $$add_platform_library(qgraphicsmagick)

LIBS += -l$$replaceLibName(utils)

include(../../qmake/VcpkgToolchain.pri)

HEADERS += \
    freeimageutils.hpp \
    qfreeimage_global.hpp

SOURCES += \
    freeimageutils.cc

include(../3rdparty/3rdparty.pri)
include(../slib.pri)

QT += widgets concurrent

DEFINES += QGRAPHICSMAGICK_LIBRARY
TARGET = $$replaceLibName(qgraphicsmagick)

LIBS += -l$$replaceLibName(utils)

HEADERS += \
    freeimageutils.hpp \
    qfreeimage_global.hpp

SOURCES += \
    freeimageutils.cc

include(../../qmake/PlatformLibraries.pri)

QT += widgets concurrent

DEFINES += GRAPHICS_LIBRARY
TARGET = $$add_platform_library(graphics)

LIBS += -l$$replaceLibName(utils)

SOURCES += \
    graphicsarcitem.cpp \
    graphicsbasicitem.cpp \
    graphicscircleitem.cpp \
    graphicslineitem.cpp \
    graphicspixmapitem.cpp \
    graphicspolygonitem.cpp \
    graphicsrectitem.cpp \
    graphicsringitem.cpp \
    graphicsrotatedrectitem.cpp \
    graphicsroundedrectitem.cc \
    graphicstextitem.cc \
    graphicsutils.cc \
    graphicsview.cc

HEADERS += \
    graphics_global.h \
    graphicsarcitem.h \
    graphicsbasicitem.h \
    graphicscircleitem.h \
    graphicslineitem.h \
    graphicspixmapitem.h \
    graphicspolygonitem.h \
    graphicsrectitem.h \
    graphicsringitem.h \
    graphicsrotatedrectitem.h \
    graphicsroundedrectitem.hpp \
    graphicstextitem.hpp \
    graphicsutils.hpp \
    graphicsview.hpp

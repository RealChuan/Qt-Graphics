include(../slib.pri)

QT += widgets concurrent

DEFINES += GRAPHICS_LIBRARY
TARGET = $$replaceLibName(graphics)

LIBS += -l$$replaceLibName(utils)

SOURCES += \
    basicgraphicsitem.cpp \
    graphics.cpp \
    graphicsarcitem.cpp \
    graphicscircleitem.cpp \
    graphicslineitem.cpp \
    graphicspixmapitem.cpp \
    graphicspolygonitem.cpp \
    graphicsrectitem.cpp \
    graphicsringitem.cpp \
    graphicsrotatedrectitem.cpp \
    graphicsroundedrectitem.cc \
    graphicstextitem.cc \
    imageview.cpp

HEADERS += \
    basicgraphicsitem.h \
    graphics.h \
    graphics_global.h \
    graphicsarcitem.h \
    graphicscircleitem.h \
    graphicslineitem.h \
    graphicspixmapitem.h \
    graphicspolygonitem.h \
    graphicsrectitem.h \
    graphicsringitem.h \
    graphicsrotatedrectitem.h \
    graphicsroundedrectitem.hpp \
    graphicstextitem.hpp \
    imageview.h

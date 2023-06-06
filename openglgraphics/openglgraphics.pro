include(../libs.pri)

QT += widgets openglwidgets

DEFINES += OPENGLGRAPHICS_LIBRARY
TARGET = $$replaceLibName(openglgraphics)

HEADERS += \
    openglgraphics_global.hpp \
    openglview.hpp

SOURCES += \
    openglview.cc

RESOURCES += \
    shader.qrc


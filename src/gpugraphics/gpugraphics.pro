include(../slib.pri)

QT += widgets openglwidgets

DEFINES += GPUGRAPHICS_LIBRARY
TARGET = $$replaceLibName(gpugraphics)

HEADERS += \
    gpugraphics_global.hpp \
    openglshaderprogram.hpp \
    openglview.hpp

SOURCES += \
    openglshaderprogram.cc \
    openglview.cc

RESOURCES += \
    shader.qrc


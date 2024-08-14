include(../slib.pri)

QT += gui widgets openglwidgets

DEFINES += GPUGRAPHICS_LIBRARY
TARGET = $$replaceLibName(gpugraphics)

HEADERS += \
    gpugraphics_global.hpp \
    openglshaderprogram.hpp \
    openglview.hpp \
    vulkanrenderer.hpp \
    vulkanview.hpp

SOURCES += \
    openglshaderprogram.cc \
    openglview.cc \
    vulkanrenderer.cc \
    vulkanview.cc

RESOURCES += \
    shader.qrc


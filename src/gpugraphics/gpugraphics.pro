include(../slib.pri)

QT += gui widgets openglwidgets

DEFINES += GPUGRAPHICS_LIBRARY
TARGET = $$replaceLibName(gpugraphics)

HEADERS += \
    gpudata.hpp \
    gpugraphics_global.hpp \
    openglshaderprogram.hpp \
    openglview.hpp

SOURCES += \
    gpudata.cc \
    openglshaderprogram.cc \
    openglview.cc

RESOURCES += \
    shader.qrc

win32{
    HEADERS += \
        vulkanrenderer.hpp \
        vulkanview.hpp

    SOURCES += \
        vulkanrenderer.cc \
        vulkanview.cc
}

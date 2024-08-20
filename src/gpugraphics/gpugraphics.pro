include(../slib.pri)

QT += gui-private gui widgets openglwidgets

DEFINES += GPUGRAPHICS_LIBRARY
TARGET = $$replaceLibName(gpugraphics)

HEADERS += \
    gpudata.hpp \
    gpugraphics_global.hpp \
    gpustr.hpp \
    openglshaderprogram.hpp \
    openglview.hpp \
    rhiview.hpp

SOURCES += \
    gpudata.cc \
    openglshaderprogram.cc \
    openglview.cc \
    rhiview.cc

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

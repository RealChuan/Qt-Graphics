include(../../qmake/PlatformLibraries.pri)

QT += gui-private gui widgets openglwidgets concurrent

DEFINES += GPUGRAPHICS_LIBRARY
TARGET = $$add_platform_library(gpugraphics)

LIBS += -l$$replaceLibName(utils)

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
#    HEADERS += \
#        vulkanrenderer.hpp \
#        vulkanview.hpp

#    SOURCES += \
#        vulkanrenderer.cc \
#        vulkanview.cc
}

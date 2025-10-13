include(../../qmake/PlatformLibraries.pri)
include(qtsingleapplication/qtsingleapplication.pri)
include(gif/gif.pri)

QT += concurrent

DEFINES += THRIDPARTY_LIBRARY
TARGET = $$add_platform_library(thirdparty)

LIBS += -l$$replaceLibName(utils)

include(../../qmake/VcpkgToolchain.pri)

HEADERS += \
    thirdparty_global.hpp

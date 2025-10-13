include(../../qmake/PlatformLibraries.pri)

QT += concurrent

DEFINES += DUMP_LIBRARY
TARGET = $$add_platform_library(dump)

LIBS += \
    -l$$replaceLibName(utils)

include(../../qmake/VcpkgToolchain.pri)

SOURCES += \
    breakpad.cc \
    crashpad.cc

HEADERS += \
    breakpad.hpp \
    crashpad.hpp \
    dump_global.hpp

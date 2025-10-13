include(../../qmake/PlatformLibraries.pri)

QT += widgets core5compat concurrent

DEFINES += UTILS_LIBRARY
TARGET = $$add_platform_library(utils)

include(../../qmake/VcpkgToolchain.pri)

SOURCES += \
    hostosinfo.cpp \
    icowriter.cc \
    imagecache.cc \
    logasync.cpp \
    logfile.cc \
    qtcassert.cpp \
    result.cpp \
    threadutils.cpp \
    utils.cc \
    validator.cc

HEADERS += \
    expected.h \
    hostosinfo.h \
    icowriter.hpp \
    imagecache.hpp \
    logasync.h \
    logfile.hpp \
    osspecificaspects.h \
    qtcassert.h \
    result.h \
    singleton.hpp \
    threadutils.h \
    utils_global.h \
    utils.hpp \
    validator.hpp

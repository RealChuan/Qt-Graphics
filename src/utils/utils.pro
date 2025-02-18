include(../slib.pri)

QT += widgets core5compat concurrent

DEFINES += UTILS_LIBRARY
TARGET = $$replaceLibName(utils)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    hostosinfo.cpp \
    imagecache.cc \
    logasync.cpp \
    logfile.cc \
    utils.cpp \
    validator.cc

HEADERS += \
    hostosinfo.h \
    imagecache.hpp \
    logasync.h \
    logfile.hpp \
    osspecificaspects.h \
    singleton.hpp \
    utils_global.h \
    utils.h \
    validator.hpp

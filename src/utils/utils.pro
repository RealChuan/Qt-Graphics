include(../slib.pri)

QT += widgets core5compat

DEFINES += UTILS_LIBRARY
TARGET = $$replaceLibName(utils)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    hostosinfo.cpp \
    logasync.cpp \
    utils.cpp \
    validator.cc

HEADERS += \
    hostosinfo.h \
    logasync.h \
    osspecificaspects.h \
    singleton.hpp \
    utils_global.h \
    utils.h \
    validator.hpp

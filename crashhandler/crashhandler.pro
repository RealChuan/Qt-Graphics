include(../libs.pri)
include(../3rdparty/3rdparty.pri)

QT += widgets

DEFINES += CRASHHANDLER_LIBRARY
TARGET = $$replaceLibName(crashhandler)

LIBS += -l$$replaceLibName(utils)
	
SOURCES += \
    breakpad.cc

HEADERS += \
    breakpad.hpp \
    crashHandler_global.h

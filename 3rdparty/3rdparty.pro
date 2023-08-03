include(3rdparty.pri)
include(../libs.pri)
include(qtsingleapplication/qtsingleapplication.pri)
include(gif/gif.pri)

DEFINES += THRIDPARTY_LIBRARY
TARGET = $$replaceLibName(thirdparty)

LIBS += -l$$replaceLibName(utils)

HEADERS += \
    breakpad.hpp \
    thirdparty_global.hpp

SOURCES += \
    breakpad.cc

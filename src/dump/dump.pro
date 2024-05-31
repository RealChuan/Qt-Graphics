include(../3rdparty/3rdparty.pri)
include(../slib.pri)

DEFINES += DUMP_LIBRARY
TARGET = $$replaceLibName(dump)

LIBS += \
    -l$$replaceLibName(utils)

SOURCES += \
    breakpad.cc \
    crashpad.cc

HEADERS += \
    breakpad.hpp \
    crashpad.hpp \
    dump_global.hpp

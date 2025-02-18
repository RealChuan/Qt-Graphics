include(../../common.pri)

QT       += core gui gui-private widgets network core5compat concurrent

TEMPLATE = app

TARGET = Qt-RhiViewer

LIBS += \
    -l$$replaceLibName(gpugraphics) \
    -l$$replaceLibName(thirdparty) \
    -l$$replaceLibName(dump) \
    -l$$replaceLibName(utils)

include(../../src/3rdparty/3rdparty.pri)

DESTDIR = $$APP_OUTPUT_PATH

SOURCES += \
    ../common/viewer.cc \
    ../common/thumbnail.cc \
    ../common/imagelistmodel.cpp \
    main.cc \
    mainwindow.cc \
    rhiviewer.cc

HEADERS += \
    ../common/viewer.hpp \
    ../common/thumbnail.hpp \
    ../common/imagelistmodel.h \
    mainwindow.hpp \
    rhiviewer.hpp

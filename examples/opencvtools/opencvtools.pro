include(../../qmake/PlatformLibraries.pri)

QT       += core gui network widgets core5compat concurrent

TEMPLATE = app

TARGET = Qt-OpenCV

LIBS += \
    -l$$replaceLibName(graphics) \
    -l$$replaceLibName(qopencv) \
    -l$$replaceLibName(thirdparty) \
    -l$$replaceLibName(dump) \
    -l$$replaceLibName(utils)

include(../../qmake/InstallCrashpad.pri)

DESTDIR = $$RUNTIME_OUTPUT_DIRECTORY

SOURCES += \
    ../common/imagelistmodel.cpp \
    ../common/thumbnail.cc \
    ../common/thumbnailcache.cc \
    ../common/viewer.cc \
    main.cc \
    mainwindow.cc \
    opencvwidget.cc

HEADERS += \
    ../common/imagelistmodel.h \
    ../common/thumbnail.hpp \
    ../common/thumbnailcache.hpp \
    ../common/viewer.hpp \
    mainwindow.hpp \
    opencvwidget.hpp

QMAKE_POST_LINK += $$setup_crashpad_handler($$RUNTIME_OUTPUT_DIRECTORY)

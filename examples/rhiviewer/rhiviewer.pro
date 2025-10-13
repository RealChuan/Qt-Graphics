include(../../qmake/PlatformLibraries.pri)

QT       += core gui gui-private widgets network core5compat concurrent

TEMPLATE = app

TARGET = Qt-RhiViewer

LIBS += \
    -l$$replaceLibName(gpugraphics) \
    -l$$replaceLibName(thirdparty) \
    -l$$replaceLibName(dump) \
    -l$$replaceLibName(utils)

include(../../qmake/InstallCrashpad.pri)

DESTDIR = $$RUNTIME_OUTPUT_DIRECTORY

SOURCES += \
    ../common/thumbnailcache.cc \
    ../common/viewer.cc \
    ../common/thumbnail.cc \
    ../common/imagelistmodel.cpp \
    main.cc \
    mainwindow.cc \
    rhiviewer.cc

HEADERS += \
    ../common/thumbnailcache.hpp \
    ../common/viewer.hpp \
    ../common/thumbnail.hpp \
    ../common/imagelistmodel.h \
    mainwindow.hpp \
    rhiviewer.hpp

QMAKE_POST_LINK += $$setup_crashpad_handler($$RUNTIME_OUTPUT_DIRECTORY)

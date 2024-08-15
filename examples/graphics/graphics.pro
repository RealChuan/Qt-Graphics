include(../../common.pri)

QT       += core gui network openglwidgets widgets core5compat concurrent

TEMPLATE = app

TARGET = Qt-Graphics

LIBS += \
    -l$$replaceLibName(gpugraphics) \
    -l$$replaceLibName(graphics) \
    -l$$replaceLibName(thirdparty) \
    -l$$replaceLibName(dump) \
    -l$$replaceLibName(utils)

include(../../src/3rdparty/3rdparty.pri)

DESTDIR = $$APP_OUTPUT_PATH

SOURCES += \
    capturewidget.cc \
    customlineitem.cc \
    drawscene.cc \
    drawwidget.cpp \
    imagecaptureview.cc \
    imagelistmodel.cpp \
    imageviewer.cpp \
    listiitemview.cc \
    main.cpp \
    mainwindow.cpp \
    maskdialog.cpp \
    openglviewer.cc \
    recordgifthread.cc \
    recordwidget.cc \
    rounddialog.cc \
    sectionalsubtitlesview.cc \
    selectionwidget.cc \
    stretchparamssettingdailog.cc \
    subtitlsplicingwidget.cc \
    viewer.cc

HEADERS += \
    capturewidget.hpp \
    customlineitem.hpp \
    drawscene.hpp \
    drawwidget.h \
    imagecaptureview.hpp \
    imagelistmodel.h \
    imageviewer.h \
    listiitemview.hpp \
    mainwindow.h \
    maskdialog.h \
    openglviewer.hpp \
    recordgifthread.hpp \
    recordwidget.hpp \
    rounddialog.hpp \
    sectionalsubtitlesview.hpp \
    selectionwidget.hpp \
    stretchparamssettingdailog.hpp \
    subtitlsplicingwidget.hpp \
    viewer.hpp

win32{
    SOURCES += vulkanviewer.cc

    HEADERS += vulkanviewer.hpp

    DEFINES += BUILD_VULKAN
}

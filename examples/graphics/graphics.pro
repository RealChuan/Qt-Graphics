include(../../qmake/PlatformLibraries.pri)

QT       += core gui network openglwidgets widgets core5compat concurrent

TEMPLATE = app

TARGET = Qt-Graphics

LIBS += \
    -l$$replaceLibName(gpugraphics) \
    -l$$replaceLibName(graphics) \
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
    capturewidget.cc \
    drawscene.cc \
    drawwidget.cpp \
    icoconverterwidget.cc \
    imagecaptureview.cc \
    imageviewer.cpp \
    listiitemview.cc \
    main.cpp \
    mainwindow.cpp \
    maskdialog.cpp \
    multiimagefileviewer.cc \
    openglviewer.cc \
    recordgifthread.cc \
    recordwidget.cc \
    rounddialog.cc \
    sectionalsubtitlesview.cc \
    selectionwidget.cc \
    sizeselectorwidget.cc \
    stretchparamssettingdailog.cc \
    subtitledividerlineitem.cc \
    subtitlsplicingwidget.cc

HEADERS += \
    ../common/thumbnailcache.hpp \
    ../common/viewer.hpp \
    ../common/thumbnail.hpp \
    ../common/imagelistmodel.h \
    capturewidget.hpp \
    drawscene.hpp \
    drawwidget.h \
    icoconverterwidget.hpp \
    imagecaptureview.hpp \
    imageviewer.h \
    listiitemview.hpp \
    mainwindow.h \
    maskdialog.h \
    multiimagefileviewer.hpp \
    openglviewer.hpp \
    recordgifthread.hpp \
    recordwidget.hpp \
    rounddialog.hpp \
    sectionalsubtitlesview.hpp \
    selectionwidget.hpp \
    sizeselectorwidget.hpp \
    stretchparamssettingdailog.hpp \
    subtitledividerlineitem.hpp \
    subtitlsplicingwidget.hpp

win32 {
#    SOURCES += vulkanviewer.cc

#    HEADERS += vulkanviewer.hpp

#    DEFINES += BUILD_VULKAN
}

win32 | unix:!macx {
    CRASHPAD_TARGET_DIR = $$RUNTIME_OUTPUT_DIRECTORY
}

macx {
    CRASHPAD_TARGET_DIR = $$RUNTIME_OUTPUT_DIRECTORY/$$member(TARGET, 0).app/Contents/MacOS
}

QMAKE_POST_LINK += $$setup_crashpad_handler($$CRASHPAD_TARGET_DIR)

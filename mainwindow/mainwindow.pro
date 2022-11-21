include(../libs.pri)

QT += widgets concurrent

DEFINES += MAINWINDOW_LIBRARY
TARGET = $$replaceLibName(mainwindow)

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(graphics)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    capturewidget.cc \
    customlineitem.cc \
    drawscene.cc \
    drawwidget.cpp \
    imagecaptureview.cc \
    imagelistmodel.cpp \
    imageviewer.cpp \
    listiitemview.cc \
    mainwindow.cpp \
    maskdialog.cpp \
    recordwidget.cc \
    rounddialog.cc \
    sectionalsubtitlesview.cc \
    selectionwidget.cc \
    stretchparamssettingdailog.cc \
    subtitlsplicingwidget.cc \
    validator.cc

HEADERS += \
    capturewidget.hpp \
    customlineitem.hpp \
    drawscene.hpp \
    drawwidget.h \
    imagecaptureview.hpp \
    imagelistmodel.h \
    imageviewer.h \
    listiitemview.hpp \
    mainwindow_global.h \
    mainwindow.h \
    maskdialog.h \
    recordwidget.hpp \
    rounddialog.hpp \
    sectionalsubtitlesview.hpp \
    selectionwidget.hpp \
    stretchparamssettingdailog.hpp \
    subtitlsplicingwidget.hpp \
    validator.hpp

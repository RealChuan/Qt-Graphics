include(../libs.pri)

QT += widgets concurrent

DEFINES += MAINWINDOW_LIBRARY
TARGET = $$replaceLibName(mainwindow)

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(graphics)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    customlineitem.cc \
    drawwidget.cpp \
    imagelistmodel.cpp \
    imageviewer.cpp \
    mainwindow.cpp \
    maskdialog.cpp \
    rounddialog.cc \
    sectionalsubtitlesview.cc \
    stretchparamssettingdailog.cc \
    subtitlsplicingwidget.cc

HEADERS += \
    customlineitem.hpp \
    drawwidget.h \
    imagelistmodel.h \
    imageviewer.h \
    mainwindow_global.h \
    mainwindow.h \
    maskdialog.h \
    rounddialog.hpp \
    sectionalsubtitlesview.hpp \
    stretchparamssettingdailog.hpp \
    subtitlsplicingwidget.hpp

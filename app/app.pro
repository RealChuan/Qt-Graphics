include(../Common.pri)

QT       += core gui network openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TEMPLATE = app

TARGET = QGraphicsTool

LIBS += -L$$APP_OUTPUT_PATH/../libs

LIBS += \
    -l$$replaceLibName(mainwindow) \
    -l$$replaceLibName(gpugraphics) \
    -l$$replaceLibName(graphics) \
    -l$$replaceLibName(thirdparty) \
    -l$$replaceLibName(utils)

include(../3rdparty/3rdparty.pri)

RC_ICONS = app.ico
#ICON     = app.icns

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp

OTHER_FILES += \
    app.ico

DESTDIR = $$APP_OUTPUT_PATH

RESOURCES += \
    resource.qrc

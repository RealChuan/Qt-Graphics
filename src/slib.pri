include(../common.pri)

TEMPLATE = lib

win32 {
DESTDIR = $$APP_OUTPUT_PATH/../libs
DLLDESTDIR = $$APP_OUTPUT_PATH
}

unix{
CONFIG += staticlib
DESTDIR = $$APP_OUTPUT_PATH/../libs
}

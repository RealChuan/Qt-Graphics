include(3rdparty.pri)
include(../libs.pri)
include(qtsingleapplication/qtsingleapplication.pri)

DEFINES += THRIDPARTY_LIBRARY
TARGET = $$replaceLibName(thirdparty)

QMAKE_TARGET_PRODUCT = ThirdParty
QMAKE_TARGET_DESCRIPTION = Third Party

HEADERS += \
    thirdparty_global.hpp

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

contains(QT_ARCH, i386) {
    BIN = bin-32
}else{
    BIN = bin-64
}

CONFIG(debug, debug|release) {
    APP_OUTPUT_PATH = $$PWD/$$BIN/Debug
}else{
    APP_OUTPUT_PATH = $$PWD/$$BIN/Release
}

INCLUDEPATH += $$PWD/
DEPENDPATH  += $$PWD/

defineReplace(replaceLibName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

isEmpty(RC_LANG): RC_LANG = 0x0004
isEmpty(VERSION): VERSION = 0.0.0.1
CONFIG += skip_target_version_ext

isEmpty(QMAKE_TARGET_COMPANY): QMAKE_TARGET_COMPANY = The Youth.
isEmpty(QMAKE_TARGET_DESCRIPTION): QMAKE_TARGET_DESCRIPTION = Qt-Graphics
isEmpty(QMAKE_TARGET_COPYRIGHT): QMAKE_TARGET_COPYRIGHT = Copyright (C) 2022 Youth.
isEmpty(QMAKE_TARGET_PRODUCT): QMAKE_TARGET_PRODUCT = Qt-Graphics
isEmpty(QMAKE_TARGET_ORIGINAL_FILENAME): QMAKE_TARGET_ORIGINAL_FILENAME = Qt-Graphics
isEmpty(QMAKE_TARGET_INTERNALNAME): QMAKE_TARGET_INTERNALNAME = Qt-Graphics
isEmpty(QMAKE_TARGET_COMMENTS): QMAKE_TARGET_COMMENTS = Qt-Graphics
isEmpty(QMAKE_TARGET_TRADEMARKS): QMAKE_TARGET_TRADEMARKS = Youth

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

# add debug info
QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

# unix add rpath
macx:QMAKE_LFLAGS += "-Wl,-rpath,@executable_path:@executable_path/../Frameworks:@executable_path/../../Frameworks"
unix:!macx:QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\':\'\$$ORIGIN/lib\':'\$$ORIGIN/../lib'"

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

INCLUDEPATH += \
    $$PWD \
    $$PWD/src/
DEPENDPATH += \
    $$PWD \
    $$PWD/src/

LIBS += \
    -L$$APP_OUTPUT_PATH \
    -L$$APP_OUTPUT_PATH/../libs

isEmpty(RC_LANG): RC_LANG = 0x0004
isEmpty(VERSION): VERSION = 0.0.1.0
CONFIG += skip_target_version_ext

isEmpty(QMAKE_TARGET_COMPANY): QMAKE_TARGET_COMPANY = The Youth.
isEmpty(QMAKE_TARGET_DESCRIPTION): QMAKE_TARGET_DESCRIPTION = Qt-App
isEmpty(QMAKE_TARGET_COPYRIGHT): QMAKE_TARGET_COPYRIGHT = Copyright (C) 2017-2024 Youth.
isEmpty(QMAKE_TARGET_PRODUCT): QMAKE_TARGET_PRODUCT = Qt-App
isEmpty(QMAKE_TARGET_ORIGINAL_FILENAME): QMAKE_TARGET_ORIGINAL_FILENAME = Qt-App
isEmpty(QMAKE_TARGET_INTERNALNAME): QMAKE_TARGET_INTERNALNAME = Qt-App
isEmpty(QMAKE_TARGET_COMMENTS): QMAKE_TARGET_COMMENTS = Qt-App
isEmpty(QMAKE_TARGET_TRADEMARKS): QMAKE_TARGET_TRADEMARKS = Youth

defineReplace(replaceLibName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

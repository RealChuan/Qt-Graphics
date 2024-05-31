win32{
    contains(QT_ARCH, i386) {
        vcpkg_path = C:/vcpkg/installed/x86-windows
    } else:contains(QT_ARCH, arm64) {
        vcpkg_path = C:/vcpkg/installed/arm64-windows
    } else {
        vcpkg_path = C:/vcpkg/installed/x64-windows
    }
}

macx{
    contains(QMAKE_APPLE_DEVICE_ARCHS, arm64) {
        vcpkg_path = /usr/local/share/vcpkg/installed/arm64-osx
    } else {
        vcpkg_path = /usr/local/share/vcpkg/installed/x64-osx
    }
}

unix:!macx{
    contains(QT_ARCH, arm64) {
        vcpkg_path = /usr/local/share/vcpkg/installed/arm64-linux
    }else{
        vcpkg_path = /usr/local/share/vcpkg/installed/x64-linux
    }
}

message("QT_ARCH: "$$QT_ARCH)
message("vcpkg_path: "$$vcpkg_path)

CONFIG(debug, debug|release) {
    LIBS += -L$$vcpkg_path/debug/lib \
            -llibbreakpad_clientd -llibbreakpadd
}else{
    LIBS += -L$$vcpkg_path/lib \
            -llibbreakpad_client -llibbreakpad
}

LIBS += -lcommon -lclient -lutil -lbase

LIBS += -lgif

INCLUDEPATH += \
    $$vcpkg_path/include \
    $$vcpkg_path/include/crashpad

win32 {
    DEFINES += NOMINMAX
    LIBS += -lAdvapi32
}

macx{
    LIBS += \
        -lbsm \
        -lmig_output

    LIBS += \
        -framework Foundation \
        -framework CoreAudio \
        -framework AVFoundation \
        -framework CoreGraphics \
        -framework OpenGL \
        -framework CoreText \
        -framework CoreImage \
        -framework AppKit \
        -framework Security \
        -framework AudioToolBox \
        -framework VideoToolBox \
        -framework CoreFoundation \
        -framework CoreMedia \
        -framework CoreVideo \
        -framework CoreServices
    #    -framework QuartzCore \
    #    -framework Cocoa \
    #    -framework VideoDecodeAcceleration
}
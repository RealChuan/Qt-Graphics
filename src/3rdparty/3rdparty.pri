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
    suffix = d
    LIBS += -L$$vcpkg_path/debug/lib
}else{
    LIBS += -L$$vcpkg_path/lib
}

LIBS += -llibbreakpad_client$$suffix -llibbreakpad$$suffix
LIBS += -lvcpkg_crashpad_client_common -lvcpkg_crashpad_client -lvcpkg_crashpad_util -lvcpkg_crashpad_base
LIBS += -lgif
LIBS += -lopencv_dnn_superres4$$suffix -lopencv_ximgproc4$$suffix -lopencv_dnn4$$suffix -lopencv_imgproc4$$suffix -lopencv_core4$$suffix
# abseil 库链接过于复杂，部分功能不支持

LIBS += -lFreeImage$$suffix

INCLUDEPATH += \
    $$vcpkg_path/include \
    $$vcpkg_path/include/crashpad \
    $$vcpkg_path/include/opencv4 \
#    $$vcpkg_path/include/GraphicsMagick

win32 {
    LIBS += -lzlib$$suffix
    DEFINES += NOMINMAX
    LIBS += -lAdvapi32
}

unix{
    LIBS += -lz
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

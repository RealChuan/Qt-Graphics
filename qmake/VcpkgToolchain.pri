# VcpkgToolchain.pri - vcpkg 依赖配置

# 包含架构检测
include(ArchitectureDetection.pri)

# 设置平台特定的 vcpkg 路径
win32 {
    vcpkg_path = C:/vcpkg/installed/$$TARGET_ARCH-windows
}

macx {
    vcpkg_path = /usr/local/share/vcpkg/installed/$$TARGET_ARCH-osx
}

unix:!macx {
    vcpkg_path = /usr/local/share/vcpkg/installed/$$TARGET_ARCH-linux
}

# 输出 vcpkg 路径信息
message("Using vcpkg path: $$vcpkg_path")

# 设置调试后缀和库路径
CONFIG(debug, debug|release) {
    suffix = d
    LIBS += -L$$vcpkg_path/debug/lib
} else {
    suffix =
    LIBS += -L$$vcpkg_path/lib
}

# Breakpad 库
LIBS += -llibbreakpad_client$$suffix -llibbreakpad$$suffix

# Crashpad 库
LIBS += -lvcpkg_crashpad_client_common -lvcpkg_crashpad_client -lvcpkg_crashpad_util -lvcpkg_crashpad_base

# gif
LIBS += -lgif

# opencv
LIBS += -lopencv_dnn_superres4$$suffix -lopencv_ximgproc4$$suffix -lopencv_dnn4$$suffix -lopencv_imgproc4$$suffix -lopencv_core4$$suffix
# abseil 库链接过于复杂，部分功能不支持

LIBS += -lFreeImage$$suffix

# 包含路径
INCLUDEPATH += \
    $$vcpkg_path/include \
    $$vcpkg_path/include/crashpad \
    $$vcpkg_path/include/opencv4

# for crashpad
win32 {
    LIBS += -lzlib$$suffix -lAdvapi32
    DEFINES += NOMINMAX
}

unix {
    LIBS += -lz
}

macx {
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

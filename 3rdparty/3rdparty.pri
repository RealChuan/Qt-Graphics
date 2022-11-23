win32{
    vcpkg_path = C:/vcpkg
    contains(QT_ARCH, i386) {
        arch = x86-windows
    }else{
        arch = x64-windows
    }
}

macx{
    vcpkg_path = /usr/local/share/vcpkg
    arch = x64-osx
}

unix:!macx{
    vcpkg_path = /usr/local/share/vcpkg
    arch = x64-linux
}

CONFIG(debug, debug|release) {
    LIBS += -L$$vcpkg_path/installed/$$arch/debug/lib \
            -llibbreakpad_clientd \
            -llibbreakpadd
}else{
    LIBS += -L$$vcpkg_path/installed/$$arch/lib \
            -llibbreakpad_client \
            -llibbreakpad
}

LIBS += -lgif

INCLUDEPATH += $$vcpkg_path/installed/$$arch/include

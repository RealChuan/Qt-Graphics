TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    src \
    examples

DISTFILES += \
    docs/** \
    .clang* \
    LICENSE \
    README*

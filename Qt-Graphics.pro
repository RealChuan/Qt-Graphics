TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    utils\
    3rdparty \
    gpugraphics \
    graphics \
    mainwindow \
    app

TRANSLATIONS += \
    translations/language.zh_cn.ts \
    translations/language.zh_en.ts

DISTFILES += \
    $$files(docs/*.png)\
    LICENSE \
    README.md

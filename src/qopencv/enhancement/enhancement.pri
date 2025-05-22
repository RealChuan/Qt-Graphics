HEADERS += \
    $$PWD/bilateralfilter.hpp \
    $$PWD/dehazed.hpp \
    $$PWD/enhancement.hpp \
    $$PWD/gammacorrection.hpp \
    $$PWD/gaussianblur.hpp \
    $$PWD/histogramequalization.hpp \
    $$PWD/linearcontrast.hpp \
    $$PWD/logtransformation.hpp \
    $$PWD/medianblur.hpp \
    $$PWD/sharpen.hpp 

SOURCES += \
    $$PWD/bilateralfilter.cc \
    $$PWD/dehazed.cc \
    $$PWD/enhancement.cc \
    $$PWD/gammacorrection.cc \
    $$PWD/gaussianblur.cc \
    $$PWD/histogramequalization.cc \
    $$PWD/linearcontrast.cc \
    $$PWD/logtransformation.cc \
    $$PWD/medianblur.cc \
    $$PWD/sharpen.cc 

# 由于 abseil 库链接过于复杂，所以只在windows下支持SuperResolution
win32{
   HEADERS += $$PWD/superresolution.hpp
   SOURCES += $$PWD/superresolution.cc
   DEFINES += LINK_DNN_SUPERRES
}

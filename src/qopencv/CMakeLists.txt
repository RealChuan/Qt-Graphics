set(PROJECT_SOURCES
    enhancement/dehazed.cc
    enhancement/dehazed.hpp
    enhancement/enhancement.cc
    enhancement/enhancement.hpp
    enhancement/gammacorrection.cc
    enhancement/gammacorrection.hpp
    enhancement/histogramequalization.cc
    enhancement/histogramequalization.hpp
    enhancement/linearcontrast.cc
    enhancement/linearcontrast.hpp
    enhancement/logtransformation.cc
    enhancement/logtransformation.hpp
    enhancement/sharpen.cc
    enhancement/sharpen.hpp
    enhancement/superresolution.cc
    enhancement/superresolution.hpp
    filter/bilateralfilter.cc
    filter/bilateralfilter.hpp
    filter/blur.cc
    filter/blur.hpp
    filter/boxfilter.cc
    filter/boxfilter.hpp
    filter/filter.cc
    filter/filter.hpp
    filter/gaussianblur.cc
    filter/gaussianblur.hpp
    filter/medianblur.cc
    filter/medianblur.hpp
    edgedetection/canny.cc
    edgedetection/canny.hpp
    edgedetection/edgedetection.cc
    edgedetection/edgedetection.hpp
    edgedetection/laplacian.cc
    edgedetection/laplacian.hpp
    edgedetection/scharr.cc
    edgedetection/scharr.hpp
    edgedetection/sobel.cc
    edgedetection/sobel.hpp
    segmentation/adaptivethreshold.cc
    segmentation/adaptivethreshold.hpp
    segmentation/segmentation.cc
    segmentation/segmentation.hpp
    segmentation/threshold.cc
    segmentation/threshold.hpp
    segmentation/watershed.cc
    segmentation/watershed.hpp
    opencvobject.cc
    opencvobject.hpp
    opencvutils.cc
    opencvutils.hpp
    qopencv_global.hpp
    qopencv.cc
    qopencv.hpp)

add_platform_library(qopencv ${PROJECT_SOURCES})
target_link_libraries(qopencv PRIVATE utils Qt::Concurrent Qt::Widgets
                                      ${OpenCV_LIBS})
target_compile_definitions(qopencv PRIVATE "LINK_DNN_SUPERRES")

if(CMAKE_HOST_WIN32)
  target_compile_definitions(qopencv PRIVATE "QOPENCV_LIBRARY")
endif()

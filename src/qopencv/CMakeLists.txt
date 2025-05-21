set(PROJECT_SOURCES
    enhancement/bilateralfilter.cc
    enhancement/bilateralfilter.hpp
    enhancement/enhancement.cc
    enhancement/enhancement.hpp
    enhancement/gaussianblur.cc
    enhancement/gaussianblur.hpp
    enhancement/histogramequalization.cc
    enhancement/histogramequalization.hpp
    enhancement/medianblur.cc
    enhancement/medianblur.hpp
    opencvobject.cc
    opencvobject.hpp
    opencvutils.cc
    opencvutils.hpp
    qopencv_global.hpp
    qopencv.cc
    qopencv.hpp)

add_custom_library(qopencv ${PROJECT_SOURCES})
target_link_libraries(qopencv PRIVATE utils Qt::Concurrent Qt::Widgets
                                      ${OpenCV_LIBS})

if(CMAKE_HOST_WIN32)
  target_compile_definitions(qopencv PRIVATE "QOPENCV_LIBRARY")
endif()

set(PROJECT_SOURCES opencvutils.cc opencvutils.hpp qopencv_global.hpp)

add_custom_library(qopencv ${PROJECT_SOURCES})
target_link_libraries(qopencv PRIVATE Qt::Gui ${OpenCV_LIBS})

if(CMAKE_HOST_WIN32)
  target_compile_definitions(qopencv PRIVATE "QOPENCV_LIBRARY")
endif()

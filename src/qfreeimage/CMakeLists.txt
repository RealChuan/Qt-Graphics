set(PROJECT_SOURCES freeimageutils.cc freeimageutils.hpp qfreeimage_global.hpp)

add_custom_library(qfreeimage ${PROJECT_SOURCES})
target_link_libraries(qfreeimage PRIVATE utils Qt::Widgets)
target_link_libraries(qfreeimage PRIVATE freeimage::FreeImage
                                         freeimage::FreeImagePlus)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(qfreeimage PRIVATE "QFREEIMAGE_LIBRARY")
endif()

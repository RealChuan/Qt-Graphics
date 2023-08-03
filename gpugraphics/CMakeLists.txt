set(PROJECT_SOURCES gpugraphics_global.hpp openglshaderprogram.cc
                    openglshaderprogram.hpp openglview.cc openglview.hpp)

qt_add_resources(SOURCES shader.qrc)

add_custom_library(gpugraphics ${PROJECT_SOURCES} ${SOURCES})
target_link_libraries(gpugraphics PRIVATE Qt6::Widgets Qt6::OpenGLWidgets)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(gpugraphics PRIVATE "GPUGRAPHICS_LIBRARY")
endif()

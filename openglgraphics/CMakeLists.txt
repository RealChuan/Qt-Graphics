set(PROJECT_SOURCES openglgraphics_global.hpp openglview.cc openglview.hpp)

qt_add_resources(SOURCES shader.qrc)

add_custom_library(openglgraphics ${PROJECT_SOURCES} ${SOURCES})
target_link_libraries(openglgraphics PRIVATE Qt6::Widgets Qt6::OpenGLWidgets)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(openglgraphics PRIVATE "OPENGLGRAPHICS_LIBRARY")
endif()

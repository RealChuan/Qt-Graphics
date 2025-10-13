set(PROJECT_SOURCES GifEncoder.cpp GifEncoder.h NeuQuant.cpp NeuQuant.h)

add_platform_library(egif ${PROJECT_SOURCES})
target_link_libraries(egif PRIVATE GIF::GIF Qt::Widgets)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(egif PRIVATE "THRIDPARTY_LIBRARY" _UNICODE UNICODE)
endif()

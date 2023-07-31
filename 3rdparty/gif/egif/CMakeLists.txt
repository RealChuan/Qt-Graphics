set(PROJECT_SOURCES GifEncoder.cpp GifEncoder.h NeuQuant.cpp NeuQuant.h)

add_custom_library(egif ${PROJECT_SOURCES})
target_link_libraries(egif PRIVATE GIF::GIF Qt6::Widgets)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(egif PRIVATE "THRIDPARTY_LIBRARY" _UNICODE UNICODE)
endif()

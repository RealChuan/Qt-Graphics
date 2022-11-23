set(PROJECT_SOURCES
      GifEncoder.cpp GifEncoder.h
      NeuQuant.cpp NeuQuant.h)

if (CMAKE_HOST_WIN32)
  add_library(egif SHARED ${PROJECT_SOURCES})
else ()
  add_library(egif ${PROJECT_SOURCES})
endif ()
target_link_libraries(egif PRIVATE GIF::GIF Qt6::Widgets)

if (CMAKE_HOST_WIN32)
  target_compile_definitions(egif PRIVATE "THRIDPARTY_LIBRARY" _UNICODE UNICODE)
endif()
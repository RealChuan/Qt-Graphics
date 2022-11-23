set(PROJECT_SOURCES
      qgifimage_p.cc qgifimage_p.h
      qgifimage.cpp qgifimage.h)

if (CMAKE_HOST_WIN32)
  add_library(qtgifimage SHARED ${PROJECT_SOURCES})
else ()
  add_library(qtgifimage ${PROJECT_SOURCES})
endif ()
target_link_libraries(qtgifimage PRIVATE GIF::GIF Qt6::Widgets)

if (CMAKE_HOST_WIN32)
  target_compile_definitions(qtgifimage PRIVATE "THRIDPARTY_LIBRARY" _UNICODE UNICODE)
endif()

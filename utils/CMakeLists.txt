set(PROJECT_SOURCES
      hostosinfo.cpp hostosinfo.h
      logasync.cpp logasync.h
      osspecificaspects.h
      singleton.hpp
      taskqueue.h
      utils_global.h
      utils.cpp utils.h)

if (CMAKE_HOST_WIN32)
  add_library(utils SHARED ${PROJECT_SOURCES} ${SOURCES})
else ()
  add_library(utils ${PROJECT_SOURCES} ${SOURCES})
endif ()
target_link_libraries(utils PRIVATE Qt6::Widgets Qt6::Core5Compat)

if (CMAKE_HOST_WIN32)
  target_compile_definitions(utils PRIVATE "UTILS_LIBRARY")
endif()

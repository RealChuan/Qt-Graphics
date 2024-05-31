set(PROJECT_SOURCES
    hostosinfo.cpp
    hostosinfo.h
    logasync.cpp
    logasync.h
    osspecificaspects.h
    singleton.hpp
    utils_global.h
    utils.cpp
    utils.h
    utilstr.h
    validator.cc
    validator.hpp)

add_custom_library(utils ${PROJECT_SOURCES})
target_link_libraries(utils PRIVATE Qt6::Widgets Qt6::Core5Compat)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(utils PRIVATE "UTILS_LIBRARY")
endif()

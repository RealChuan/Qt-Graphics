set(PROJECT_SOURCES
    hostosinfo.cpp
    hostosinfo.h
    icowriter.cc
    icowriter.hpp
    imagecache.cc
    imagecache.hpp
    logasync.cpp
    logasync.h
    logfile.cc
    logfile.hpp
    osspecificaspects.h
    singleton.hpp
    utils_global.h
    utils.cpp
    utils.h
    utilstr.h
    validator.cc
    validator.hpp)

add_custom_library(utils ${PROJECT_SOURCES})
target_link_libraries(utils PRIVATE Qt::Concurrent Qt::Widgets Qt::Core5Compat)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(utils PRIVATE "UTILS_LIBRARY")
endif()

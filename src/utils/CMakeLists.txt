set(PROJECT_SOURCES
    appdata.hpp
    expected.h
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
    qtcassert.cpp
    qtcassert.h
    result.cpp
    result.h
    singleton.hpp
    threadutils.cpp
    threadutils.h
    utils_global.h
    utils.cc
    utils.hpp
    utilstr.h
    validator.cc
    validator.hpp)

add_platform_library(utils ${PROJECT_SOURCES})
target_link_libraries(utils PRIVATE Qt::Concurrent Qt::Widgets Qt::Core5Compat
                                    tl::expected)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(utils PRIVATE "UTILS_LIBRARY")
endif()

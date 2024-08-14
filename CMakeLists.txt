cmake_minimum_required(VERSION 3.25.1)

include(cmake/vcpkg.cmake)
include(cmake/qt.cmake)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(utils)
include(qt_utils)

project(
  Qt-Graphics
  VERSION 0.0.1
  DESCRIPTION "This is a project for qt graphics framework"
  HOMEPAGE_URL "https://github.com/RealChuan/Qt-Graphics"
  LANGUAGES CXX)

include(cmake/common.cmake)

find_package(Qt6 REQUIRED COMPONENTS Gui Widgets Network Core5Compat Concurrent
                                     OpenGLWidgets)

qt_standard_project_setup()
# qt_standard_project_setup will set CMAKE_RUNTIME_OUTPUT_DIRECTORY, we need to
# set it back, and use EXECUTABLE_OUTPUT_PATH
unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

find_package(unofficial-breakpad CONFIG REQUIRED)
if(unofficial-breakpad_FOUND)
  message(STATUS "found unofficial-breakpad")
endif()
find_package(crashpad CONFIG REQUIRED)
if(crashpad_FOUND)
  message(STATUS "found crashpad")
endif()
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
  message(STATUS "found Vulkan")
endif()

find_package(GIF REQUIRED)
if(GIF_FOUND)
  message(STATUS "found GIF")
endif()

include_directories(src)
add_subdirectory(src)
add_subdirectory(examples)

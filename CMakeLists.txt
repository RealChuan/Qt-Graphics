cmake_minimum_required(VERSION 3.25.1)

include(cmake/macOSDeploymentTarget.cmake)
include(cmake/VcpkgToolchain.cmake)
include(cmake/QtSetup.cmake)

project(
  Qt-Graphics
  VERSION 0.0.1
  DESCRIPTION "This is a project for qt graphics framework"
  HOMEPAGE_URL "https://github.com/RealChuan/Qt-Graphics"
  LANGUAGES CXX)

option(BUILD_VULKAN "build vulkan" OFF)

include(cmake/ProjectSettings.cmake)
include(cmake/InstallConfig.cmake)
include(cmake/PlatformLibraries.cmake)
include(cmake/InstallCrashpad.cmake)

find_package(
  Qt6 REQUIRED
  COMPONENTS Gui
             GuiPrivate
             Widgets
             Network
             Core5Compat
             Concurrent
             OpenGLWidgets)

qt_standard_project_setup()

include(cmake/QtUtilities.cmake)

find_package(unofficial-breakpad CONFIG REQUIRED)
if(unofficial-breakpad_FOUND)
  message(STATUS "found unofficial-breakpad")
endif()
find_package(crashpad CONFIG REQUIRED)
if(crashpad_FOUND)
  message(STATUS "found crashpad")
endif()
find_package(tl-expected CONFIG REQUIRED)
if(tl-expected_FOUND)
  message(STATUS "found tl-expected")
endif()
find_package(freeimage CONFIG REQUIRED)
if(freeimage_FOUND)
  message(STATUS "found freeimage")
endif()
find_package(GIF REQUIRED)
if(GIF_FOUND)
  message(STATUS "found GIF")
endif()
find_package(OpenCV CONFIG REQUIRED)
if(OpenCV_FOUND)
  message(STATUS "found OpenCV")
endif()

message(STATUS "BUILD_VULKAN: ${BUILD_VULKAN}")
if(BUILD_VULKAN)
  find_package(Vulkan REQUIRED)
  if(Vulkan_FOUND)
    message(STATUS "found Vulkan")
  endif()
endif()

include_directories(src)

add_subdirectory(src)
add_subdirectory(examples)

include(cmake/CMakeDebugInfo.cmake)

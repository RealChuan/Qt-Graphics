cmake_minimum_required(VERSION 3.16)

if(CMAKE_HOST_WIN32)
  set(CMAKE_TOOLCHAIN_FILE
      "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
elseif(CMAKE_HOST_APPLE)
  set(CMAKE_TOOLCHAIN_FILE
      "/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
elseif(CMAKE_HOST_UNIX)
  set(CMAKE_TOOLCHAIN_FILE
      "/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
endif()

if(CMAKE_HOST_WIN32)
  list(APPEND CMAKE_PREFIX_PATH "C:\\Qt\\6.5.2\\msvc2019_64")
elseif(CMAKE_HOST_APPLE)

elseif(CMAKE_HOST_UNIX)
  list(APPEND CMAKE_PREFIX_PATH "/opt/Qt/6.5.2/gcc_64")
endif()

project(
  Qt-Ffmpeg
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# qt_standard_project_setup() 这个识别不了

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CURRENT_SOURCE_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_DEBUG_POSTFIX d)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE
      RelWithDebInfo
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(CURRENT_PLATFORM "-64")
else()
  set(CURRENT_PLATFORM "-32")
endif()

message(STATUS "Current Platform is ${CURRENT_PLATFORM}")

# 设定可执行二进制文件的目录
set(EXECUTABLE_OUTPUT_PATH
    ${PROJECT_SOURCE_DIR}/bin${CURRENT_PLATFORM}/${CMAKE_BUILD_TYPE}) # 源文件目录
# 设定存放编译出来的库文件的目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin${CURRENT_PLATFORM}/libs)
# 并且把该目录设为连接目录
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${PROJECT_SOURCE_DIR})

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Core5Compat Concurrent
                                     OpenGLWidgets)

find_package(unofficial-breakpad CONFIG REQUIRED)
if(unofficial-breakpad_FOUND)
  message(STATUS "found unofficial-breakpad")
endif()
find_package(GIF REQUIRED)
if(GIF_FOUND)
  message(STATUS "found GIF")
endif()

function(add_custom_library target_name)
  if(CMAKE_HOST_WIN32)
    add_library(${target_name} SHARED ${ARGN})
    add_custom_command(
      TARGET ${target_name}
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${target_name}>
        ${EXECUTABLE_OUTPUT_PATH}/$<TARGET_FILE_NAME:${target_name}>)
  else()
    add_library(${target_name} ${ARGN})
  endif()
endfunction(add_custom_library)

add_subdirectory(utils)
add_subdirectory(3rdparty)
add_subdirectory(crashhandler)
add_subdirectory(graphics)
add_subdirectory(openglgraphics)
add_subdirectory(mainwindow)
add_subdirectory(app)

if(CMAKE_HOST_WIN32)
  set(CMAKE_TOOLCHAIN_FILE
      "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
elseif(CMAKE_HOST_APPLE)
  set(CMAKE_TOOLCHAIN_FILE
      "/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
elseif(CMAKE_HOST_LINUX)
  set(CMAKE_TOOLCHAIN_FILE
      "/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
endif()

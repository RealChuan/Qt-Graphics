add_subdirectory(gif)
add_subdirectory(qtlockedfile)

set(PROJECT_SOURCES
    qtsingleapplication/qtsingleapplication.cpp
    qtsingleapplication/qtsingleapplication.h
    qtsingleapplication/qtlocalpeer.cpp
    qtsingleapplication/qtlocalpeer.h
    breakpad.cc
    breakpad.hpp
    thirdparty_global.hpp)

add_custom_library(thirdparty ${PROJECT_SOURCES})
target_link_libraries(
  thirdparty
  PRIVATE shared_qtlockedfile utils Qt6::Network Qt6::Widgets
          unofficial::breakpad::libbreakpad
          unofficial::breakpad::libbreakpad_client)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(thirdparty PRIVATE "THRIDPARTY_LIBRARY")
endif()

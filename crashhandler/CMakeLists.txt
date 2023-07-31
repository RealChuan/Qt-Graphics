set(PROJECT_SOURCES breakpad.cc breakpad.hpp crashhandler_global.h)

add_custom_library(crashhandler ${PROJECT_SOURCES})
target_link_libraries(
  crashhandler PRIVATE Qt6::Widgets utils unofficial::breakpad::libbreakpad
                       unofficial::breakpad::libbreakpad_client)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(crashhandler PRIVATE "CRASHHANDLER_LIBRARY")
endif()

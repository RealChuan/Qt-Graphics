set(PROJECT_SOURCES breakpad.hpp breakpad.cc crashpad.hpp crashpad.cc
                    dump_global.hpp)
add_custom_library(dump ${PROJECT_SOURCES})
target_link_libraries(
  dump PRIVATE utils Qt6::Widgets unofficial::breakpad::libbreakpad
               unofficial::breakpad::libbreakpad_client crashpad::crashpad)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(dump PRIVATE "DUMP_LIBRARY")
endif()

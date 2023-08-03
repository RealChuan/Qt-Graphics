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

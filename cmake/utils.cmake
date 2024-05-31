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

function(add_custom_plugin target_name)
  add_library(${target_name} SHARED ${ARGN})
  add_custom_command(
    TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${target_name}>
            ${EXECUTABLE_OUTPUT_PATH}/plugins/$<TARGET_FILE_NAME:${target_name}>
  )
  add_rpath(${target_name})
endfunction(add_custom_plugin)

function(add_share_library target_name)
  add_library(${target_name} SHARED ${ARGN})
  add_custom_command(
    TARGET ${target_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${target_name}>
            ${EXECUTABLE_OUTPUT_PATH}/$<TARGET_FILE_NAME:${target_name}>)
  add_rpath(${target_name})
endfunction(add_share_library)

function(add_rpath target_name)
  if(CMAKE_HOST_APPLE)
    set_target_properties(
      ${target_name}
      PROPERTIES LINK_FLAGS "-Wl,-rpath,./:./../Frameworks:./../../Frameworks")
  elseif(CMAKE_HOST_LINUX)
    set_target_properties(${target_name}
                          PROPERTIES LINK_FLAGS "-Wl,-rpath,./:./lib:./../lib")
  endif()
endfunction()

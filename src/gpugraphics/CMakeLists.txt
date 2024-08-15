set(PROJECT_SOURCES gpugraphics_global.hpp openglshaderprogram.cc
                    openglshaderprogram.hpp openglview.cc openglview.hpp)

if(BUILD_VULKAN)
  list(APPEND PROJECT_SOURCES vulkanrenderer.cc vulkanrenderer.hpp
       vulkanview.cc vulkanview.hpp)
endif()

qt_add_resources(SOURCES shader.qrc)

add_custom_library(gpugraphics ${PROJECT_SOURCES} ${SOURCES})
target_link_libraries(gpugraphics PRIVATE Qt6::Gui Qt6::Widgets
                                          Qt6::OpenGLWidgets)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(gpugraphics PRIVATE "GPUGRAPHICS_LIBRARY")
endif()

if(BUILD_VULKAN)
  file(GLOB_RECURSE GLSL_SOURCE_FILES "shader/vulkan*")

  foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/vulkan/${FILE_NAME}.spv")
    add_custom_command(
      OUTPUT ${SPIRV}
      COMMAND ${CMAKE_COMMAND} -E make_directory
              "${PROJECT_BINARY_DIR}/shaders/vulkan/"
      COMMAND glslc ${GLSL} -o ${SPIRV}
      DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
  endforeach(GLSL)

  add_custom_target(TXShaders DEPENDS ${SPIRV_BINARY_FILES})

  add_dependencies(gpugraphics TXShaders)

  add_custom_command(
    TARGET gpugraphics
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "${EXECUTABLE_OUTPUT_PATH}/vulkan_shader/"
    COMMAND
      ${CMAKE_COMMAND} -E copy_directory "${PROJECT_BINARY_DIR}/shaders/vulkan"
      "${EXECUTABLE_OUTPUT_PATH}/vulkan_shader")
endif()

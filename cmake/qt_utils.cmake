function(add_translations OUTPUT_NAME)
  # 转换输出名称为小写
  string(TOLOWER ${OUTPUT_NAME} Lower_Output_Name)

  # 添加翻译文件
  qt_add_translations(
    ${OUTPUT_NAME}
    TS_FILE_BASE
    ${Lower_Output_Name}
    TS_FILE_DIR
    translations
    INCLUDE_DIRECTORIES
    directory
    ${PROJECT_SOURCE_DIR}/src)

  # 确定翻译文件的输出位置
  if(CMAKE_HOST_APPLE)
    set(output_location
        "${EXECUTABLE_OUTPUT_PATH}/${OUTPUT_NAME}.app/Contents/Resources/translations"
    )
  else()
    set(output_location "${EXECUTABLE_OUTPUT_PATH}/translations")
  endif()

  # 查找所有的翻译文件(.ts)
  file(GLOB_RECURSE TS_FILES "${PROJECT_SOURCE_DIR}/translations/*.ts")

  # 设置翻译文件的输出位置
  foreach(ts_file ${TS_FILES})
    set_source_files_properties("${ts_file}" PROPERTIES OUTPUT_LOCATION
                                                        "${output_location}")
  endforeach()
endfunction()

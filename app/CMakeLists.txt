qt_add_resources(SOURCES resource.qrc)

if(CMAKE_HOST_WIN32)
  qt_add_executable(QGraphicsTool MANUAL_FINALIZATION main.cpp app.rc)
else()
  qt_add_executable(QGraphicsTool MANUAL_FINALIZATION main.cpp)
endif()

target_link_libraries(QGraphicsTool PRIVATE mainwindow thirdparty utils
                                            Qt6::Widgets)
qt_finalize_executable(QGraphicsTool)

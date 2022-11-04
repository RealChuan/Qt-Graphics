qt_add_resources(SOURCES resource.qrc)

qt_add_executable(QPictureTool MANUAL_FINALIZATION main.cpp ${SOURCES})
target_link_libraries(QPictureTool PRIVATE Qt6::Widgets
    shared_qtsingleapplication utils crashhandler graphics mainwindow)
qt_finalize_executable(QPictureTool)

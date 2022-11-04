qt_add_resources(SOURCES resource.qrc)

qt_add_executable(QGraphicsTool MANUAL_FINALIZATION main.cpp ${SOURCES})
target_link_libraries(QGraphicsTool PRIVATE Qt6::Widgets
    shared_qtsingleapplication utils crashhandler graphics mainwindow)
qt_finalize_executable(QGraphicsTool)

set(PROJECT_SOURCES
    ../common/viewer.cc
    ../common/viewer.hpp
    ../common/imagelistmodel.cpp
    ../common/imagelistmodel.h
    main.cc
    mainwindow.cc
    mainwindow.hpp
    rhiviewer.cc
    rhiviewer.hpp)

qt_add_executable(Qt-RhiViewer MANUAL_FINALIZATION ${PROJECT_SOURCES})
target_link_libraries(
  Qt-RhiViewer
  PRIVATE gpugraphics
          thirdparty
          dump
          utils
          Qt6::GuiPrivate
          Qt6::Gui
          Qt6::Widgets
          Qt6::Network
          Qt6::Concurrent)
qt_finalize_executable(Qt-RhiViewer)

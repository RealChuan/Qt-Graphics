set(PROJECT_SOURCES
    ../common/imagelistmodel.cpp
    ../common/imagelistmodel.h
    ../common/thumbnail.cc
    ../common/thumbnail.hpp
    ../common/thumbnailcache.cc
    ../common/thumbnailcache.hpp
    ../common/viewer.cc
    ../common/viewer.hpp
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
          Qt::GuiPrivate
          Qt::Gui
          Qt::Widgets
          Qt::Network
          Qt::Concurrent)
qt_finalize_executable(Qt-RhiViewer)

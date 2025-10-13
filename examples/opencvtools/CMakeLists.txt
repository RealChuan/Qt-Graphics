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
    opencvwidget.cc
    opencvwidget.hpp)

qt_add_executable(Qt-OpenCV MANUAL_FINALIZATION ${PROJECT_SOURCES})
target_link_libraries(
  Qt-OpenCV
  PRIVATE graphics
          qopencv
          thirdparty
          dump
          utils
          Qt::Concurrent
          Qt::Gui
          Qt::Widgets
          Qt::OpenGLWidgets
          Qt::Network
          Qt::Concurrent
          ${OpenCV_LIBS}
          tl::expected)
qt_finalize_executable(Qt-OpenCV)

setup_crashpad_handler("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

set(PROJECT_SOURCES
    capturewidget.cc
    capturewidget.hpp
    customlineitem.cc
    customlineitem.hpp
    drawscene.cc
    drawscene.hpp
    drawwidget.cpp
    drawwidget.h
    imagecaptureview.cc
    imagecaptureview.hpp
    imagelistmodel.cpp
    imagelistmodel.h
    imageviewer.cpp
    imageviewer.h
    listiitemview.cc
    listiitemview.hpp
    mainwindow_global.h
    mainwindow.cpp
    mainwindow.h
    maskdialog.cpp
    maskdialog.h
    openglviewer.cc
    openglviewer.hpp
    recordgifthread.cc
    recordgifthread.hpp
    recordwidget.cc
    recordwidget.hpp
    rounddialog.cc
    rounddialog.hpp
    sectionalsubtitlesview.cc
    sectionalsubtitlesview.hpp
    selectionwidget.cc
    selectionwidget.hpp
    stretchparamssettingdailog.cc
    stretchparamssettingdailog.hpp
    subtitlsplicingwidget.cc
    subtitlsplicingwidget.hpp
    validator.cc
    validator.hpp
    viewer.cc
    viewer.hpp)

add_custom_library(mainwindow ${PROJECT_SOURCES})
target_link_libraries(
  mainwindow
  PRIVATE Qt6::Widgets
          Qt6::Concurrent
          Qt6::OpenGLWidgets
          utils
          graphics
          gpugraphics
          egif)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(mainwindow PRIVATE "MAINWINDOW_LIBRARY")
endif()

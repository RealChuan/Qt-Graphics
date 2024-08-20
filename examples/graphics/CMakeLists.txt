set(PROJECT_SOURCES
    ../common/viewer.cc
    ../common/viewer.hpp
    ../common/imagelistmodel.cpp
    ../common/imagelistmodel.h
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
    imageviewer.cpp
    imageviewer.h
    listiitemview.cc
    listiitemview.hpp
    main.cpp
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
    subtitlsplicingwidget.hpp)

if(BUILD_VULKAN)
  list(APPEND PROJECT_SOURCES vulkanviewer.cc vulkanviewer.hpp)
endif()
qt_add_executable(Qt-Graphics MANUAL_FINALIZATION ${PROJECT_SOURCES})
if(BUILD_VULKAN)
  target_compile_definitions(Qt-Graphics PRIVATE "BUILD_VULKAN")
endif()

target_link_libraries(
  Qt-Graphics
  PRIVATE graphics
          gpugraphics
          thirdparty
          dump
          utils
          Qt6::Gui
          Qt6::Widgets
          Qt6::OpenGLWidgets
          Qt6::Network
          Qt6::Concurrent
          egif)
qt_finalize_executable(Qt-Graphics)

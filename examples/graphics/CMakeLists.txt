set(PROJECT_SOURCES
    ../common/imagelistmodel.cpp
    ../common/imagelistmodel.h
    ../common/thumbnail.cc
    ../common/thumbnail.hpp
    ../common/thumbnailcache.cc
    ../common/thumbnailcache.hpp
    ../common/viewer.cc
    ../common/viewer.hpp
    capturewidget.cc
    capturewidget.hpp
    customlineitem.cc
    customlineitem.hpp
    drawscene.cc
    drawscene.hpp
    drawwidget.cpp
    drawwidget.h
    icoconverterwidget.cc
    icoconverterwidget.hpp
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
    multiimagefileviewer.cc
    multiimagefileviewer.hpp
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
    sizeselectorwidget.cc
    sizeselectorwidget.hpp
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
          Qt::Gui
          Qt::Widgets
          Qt::OpenGLWidgets
          Qt::Network
          Qt::Concurrent
          egif
          tl::expected)
qt_finalize_executable(Qt-Graphics)

setup_crashpad_handler("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

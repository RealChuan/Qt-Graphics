set(PROJECT_SOURCES
        capturewidget.cc capturewidget.hpp
        customlineitem.cc customlineitem.hpp
        drawscene.cc drawscene.hpp
        drawwidget.cpp drawwidget.h
        imagecaptureview.cc imagecaptureview.hpp
        imagelistmodel.cpp imagelistmodel.h
        imageviewer.cpp imageviewer.h
        listiitemview.cc listiitemview.hpp
        mainwindow_global.h
        mainwindow.cpp mainwindow.h
        maskdialog.cpp maskdialog.h
        recordwidget.cc recordwidget.hpp
        rounddialog.cc rounddialog.hpp
        sectionalsubtitlesview.cc sectionalsubtitlesview.hpp
        selectionwidget.cc selectionwidget.hpp
        stretchparamssettingdailog.cc stretchparamssettingdailog.hpp
        subtitlsplicingwidget.cc subtitlsplicingwidget.hpp
        validator.cc validator.hpp)

if (CMAKE_HOST_WIN32)
  add_library(mainwindow SHARED ${PROJECT_SOURCES} ${SOURCES})
else ()
  add_library(mainwindow ${PROJECT_SOURCES} ${SOURCES})
endif ()
target_link_libraries(mainwindow PRIVATE Qt6::Widgets Qt6::Concurrent utils graphics)

if (CMAKE_HOST_WIN32)
  target_compile_definitions(mainwindow PRIVATE "MAINWINDOW_LIBRARY")
endif()

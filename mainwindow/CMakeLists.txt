set(PROJECT_SOURCES
        drawwidget.cpp drawwidget.h
        imagelistmodel.cpp imagelistmodel.h
        imageviewer.cpp imageviewer.h
        mainwindow_global.h
        mainwindow.cpp mainwindow.h
        maskdialog.cpp maskdialog.h
        rounddialog.cc rounddialog.hpp
        stretchparamssettingdailog.cc stretchparamssettingdailog.hpp)

if (CMAKE_HOST_WIN32)
  add_library(mainwindow SHARED ${PROJECT_SOURCES} ${SOURCES})
else ()
  add_library(mainwindow ${PROJECT_SOURCES} ${SOURCES})
endif ()
target_link_libraries(mainwindow PRIVATE Qt6::Widgets Qt6::Concurrent utils graphics)

if (CMAKE_HOST_WIN32)
  target_compile_definitions(mainwindow PRIVATE "MAINWINDOW_LIBRARY")
endif()

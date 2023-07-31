set(PROJECT_SOURCES
    basicgraphicsitem.cpp
    basicgraphicsitem.h
    graphics_global.h
    graphics.cpp
    graphics.h
    graphicsarcitem.cpp
    graphicsarcitem.h
    graphicscircleitem.cpp
    graphicscircleitem.h
    graphicslineitem.cpp
    graphicslineitem.h
    graphicspixmapitem.cpp
    graphicspixmapitem.h
    graphicspolygonitem.cpp
    graphicspolygonitem.h
    graphicsrectitem.cpp
    graphicsrectitem.h
    graphicsringitem.cpp
    graphicsringitem.h
    graphicsrotatedrectitem.cpp
    graphicsrotatedrectitem.h
    graphicstextitem.cc
    graphicstextitem.hpp
    imageview.cpp
    imageview.h)

add_custom_library(graphics ${PROJECT_SOURCES})
target_link_libraries(graphics PRIVATE Qt6::Widgets)

if(CMAKE_HOST_WIN32)
  target_compile_definitions(graphics PRIVATE "GRAPHICS_LIBRARY")
endif()

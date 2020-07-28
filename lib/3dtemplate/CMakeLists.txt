cmake_minimum_required(VERSION 3.1)

project(3dtemplate)


set(HEADERS
        include/glCommon.h
        include/glUtils.h
        include/Attribute.h
        include/Model.h
        include/Scene.h
        include/Texture.h
        include/Renderer.h
        include/VPHandler.h
        include/RenderOptions.h
        include/Mesh.h
        include/trackball.h
        lib/glad/include/glad/glad.h
        )


set(SOURCES
        model/Attribute.cpp
        model/Model.cpp
        model/Scene.cpp
        model/Texture.cpp
        renderers/VPHandler.cpp
        mesh/Mesh.cpp
        trackball/trackball.cpp
        lib/glad/src/glad.c
        )


set(LIBRARIES
        stb
        utils
        tinyobjloader
        )

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ./include)

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor assimp)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

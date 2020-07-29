cmake_minimum_required(VERSION 3.1)

project(3dtemplate)

add_subdirectory(lib/glad)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)


set(HEADERS
        src/glCommon.h
        src/glUtils.h
        src/Attribute.h
        src/Model.h
        src/Scene.h
        src/Texture.h
        src/Renderer.h
        src/VPHandler.h
        src/RenderOptions.h
        src/Mesh.h
        src/TrackballCamera.h
        )


set(SOURCES
        src/Attribute.cpp
        src/Model.cpp
        src/Scene.cpp
        src/Texture.cpp
        src/VPHandler.cpp
        src/Mesh.cpp
        src/TrackballCamera.cpp
        )


set(LIBRARIES
        stb
        utils
        tinyobjloader
        glad
        glfw3
        glm
        )

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} GL GLU X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor assimp)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

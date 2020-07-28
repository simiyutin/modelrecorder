cmake_minimum_required(VERSION 3.14)
project(modelrecorder)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE RELEASE)

add_subdirectory(lib/stb)
add_subdirectory(lib/utils)
add_subdirectory(lib/tinyobjloader)
add_subdirectory(lib/3dtemplate)

set(HEADERS
        renderers/PositioningRenderer.h
        )


set(SOURCES
        main.cpp
        renderers/PositioningRenderer.cpp
        )


set(LIBRARIES
        stb
        utils
        tinyobjloader
        3dtemplate
        )

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor assimp)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
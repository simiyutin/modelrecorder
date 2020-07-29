cmake_minimum_required(VERSION 3.1)

project(glad)


set(HEADERS
        include/glad/glad.h
        )


set(SOURCES
        src/glad.c
        )


set(LIBRARIES

        )

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

cmake_minimum_required(VERSION 3.1)

project(stb)

set(HEADERS
        stb_image.h
        stb_image_write.h
        stb_image_wrapper.h
        )

set(SOURCES
        stb_image.cpp
        stb_image_write.cpp
        )

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ./)
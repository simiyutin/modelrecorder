cmake_minimum_required(VERSION 3.1)

project(tinyobjloader)

set(HEADERS
        tiny_obj_loader.h
        )

set(SOURCES
        tiny_obj_loader.cpp
        )

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ./)
cmake_minimum_required(VERSION 3.1)

project(utils CXX)

set(HEADERS
        contains.h
        glmUtils.h
        io.h
        )

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ./)
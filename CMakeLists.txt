cmake_minimum_required(VERSION 3.22.1)

#project name
project (Lang)

file(GLOB SOURCES
    *.h
    *.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Lang.x86_64")

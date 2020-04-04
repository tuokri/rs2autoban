cmake_minimum_required(VERSION 3.15)
project(rs2autoban)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

add_compile_options("/W4" "/std:c++latest"
        "$<$<CONFIG:RELEASE>:/O2;-DQT_NO_DEBUG_OUTPUT>")

find_package(Qt5 COMPONENTS Core Sql REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src)

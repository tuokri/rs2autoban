cmake_minimum_required(VERSION 3.15)
project(rs2autoban)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(/W4 /std:c++latest)

find_package(Qt5 COMPONENTS Core REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(Qt5Core_COMPILE_DEFINITIONS console)

add_executable(
        rs2autoban
        main.cpp
        Firewall.hpp
        Firewall.cpp
        Utils.hpp
        Utils.cpp
        LogWatcher.cpp
        LogWatcher.hpp
)

target_link_libraries(
        rs2autoban
        ole32.lib
        oleaut32.lib
        Qt5::Core
)

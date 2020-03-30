cmake_minimum_required(VERSION 3.15)
project(rs2autoban)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(/W4)

add_executable(
        rs2autoban
        main.cpp
        Firewall.hpp
        Firewall.cpp
        Utils.hpp
        Utils.cpp
)

target_link_libraries(
        rs2autoban
        ole32.lib
        oleaut32.lib
)

cmake_minimum_required(VERSION 3.15)
project(rs2autoban)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(/W4)

add_executable(
        rs2autoban
        main.cpp
        Firewall.cpp
        Firewall.hpp
        Utils.hpp
)

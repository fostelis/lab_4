cmake_minimum_required(VERSION 3.20)
project(labs_2_3_4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    include/lab2
    include/lab3
    include/lab4
    tests
)

add_executable(labs_2_3_4
    src/main.cpp
    tests/tests_lab2.cpp
    tests/tests_lab3.cpp
    tests/tests_lab4.cpp
)

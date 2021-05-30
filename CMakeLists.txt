cmake_minimum_required(VERSION 3.17.1)

project(e21)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ETHSNARKS_DISABLE_TESTS ON)
set(ETHSNARKS_DISABLE_PINOCCHIO ON)
set(DEBUG OFF)

include_directories(./libs .)

add_subdirectory(libs)

add_subdirectory(src)



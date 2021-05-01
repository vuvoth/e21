cmake_minimum_required(VERSION 3.17.1)

project(e21)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)



#gtest_discover_tests(main_test)

add_subdirectory( ./libs )

add_subdirectory(src)

add_executable(
    main
    src/main.cpp
)

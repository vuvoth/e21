cmake_minimum_required(VERSION 3.17.1)

project(e21)

set(CMAKE_CXX_STANDARD)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    main
    src/main.cpp
)

#target_link_libraries(
#    main_test
#    gtest_main
#)

include(GoogleTest)

add_subdirectory(src)
#gtest_discover_tests(main_test)



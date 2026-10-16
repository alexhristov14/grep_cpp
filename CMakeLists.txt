cmake_minimum_required(VERSION 3.20)

project(grep_cpp
    VERSION 0.1.0
    DESCRIPTION "A grep clone in cpp"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(GREP_BUILD_TESTS "Build unit tests" ON)
option(GREP_BUILD_BENCHMARKS "Build benchmarks" ON)

# Core logic shared by the executable, tests and benchmarks
add_library(grep_core STATIC src/algorithms.cpp src/highlight.cpp)

target_include_directories(grep_core
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(grep_core PUBLIC cxx_std_20)

add_library(grep_warnings INTERFACE)
target_compile_options(grep_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Wshadow -Wconversion>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
target_link_libraries(grep_core PRIVATE grep_warnings)

add_executable(grep_cpp src/main.cpp)
target_link_libraries(grep_cpp PRIVATE grep_core grep_warnings)

install(TARGETS grep_cpp RUNTIME DESTINATION bin)

include(FetchContent)

if(GREP_BUILD_TESTS)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)

    add_executable(grep_tests tests/algorithms_test.cpp tests/highlight_test.cpp)
    target_link_libraries(grep_tests PRIVATE grep_core grep_warnings GTest::gtest_main)
    gtest_discover_tests(grep_tests)
endif()

if(GREP_BUILD_BENCHMARKS)
    FetchContent_Declare(benchmark
        URL https://github.com/google/benchmark/archive/refs/tags/v1.9.1.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(benchmark)

    add_executable(grep_bench benchmarks/grep_bench.cpp)
    target_link_libraries(grep_bench PRIVATE grep_core grep_warnings benchmark::benchmark_main)
endif()

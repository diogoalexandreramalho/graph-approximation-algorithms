cmake_minimum_required(VERSION 3.16)

project(graph_approx
    VERSION 0.1.0
    DESCRIPTION "Approximation algorithms for large graph metrics"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

add_library(graph_approx
    src/apl.cpp
    src/betweenness.cpp
    src/clustering.cpp
    src/graph.cpp
)
target_include_directories(graph_approx
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_options(graph_approx PRIVATE
    -Wall -Wextra -Wpedantic
)

add_executable(graph-approx apps/main.cpp)
target_link_libraries(graph-approx PRIVATE graph_approx)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
    enable_testing()
    add_subdirectory(tests)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/CMakeLists.txt)
    add_subdirectory(benchmarks)
endif()

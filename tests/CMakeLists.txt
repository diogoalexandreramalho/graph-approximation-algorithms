include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(graph_approx_tests
    test_graph.cpp
    test_clustering.cpp
    test_betweenness.cpp
    test_mincut.cpp
)
target_link_libraries(graph_approx_tests PRIVATE
    graph_approx
    GTest::gtest_main
)
target_include_directories(graph_approx_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(graph_approx_tests)

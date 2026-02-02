# Process Stats Tests

add_executable(process_stats_tests
    test_process_stats.cpp
)

target_link_libraries(process_stats_tests PRIVATE
    process_stats
    GTest::gtest
    GTest::gtest_main
    Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(process_stats_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

gtest_discover_tests(process_stats_tests)

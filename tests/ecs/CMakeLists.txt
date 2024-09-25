add_executable(
    componets_manager_tests
    componentManagerTests.cpp
)

target_link_libraries(
    componets_manager_tests
    PRIVATE
    GTest::gtest_main
    glm
    ecs
)

gtest_discover_tests(componets_manager_tests)
enable_compiler_warings(componets_manager_tests)

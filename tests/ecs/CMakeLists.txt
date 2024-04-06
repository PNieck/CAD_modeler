add_executable(
    componets_manager_tests
    componentManagerTests.cpp
)

target_link_libraries(
    componets_manager_tests
    PRIVATE
    GTest::gtest_main
    modeler_lib
    glm
)

gtest_discover_tests(componets_manager_tests)

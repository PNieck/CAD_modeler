add_executable(
    plane_tests
    planeTests.cpp
)

target_link_libraries(
    plane_tests
    PRIVATE
    GTest::gtest_main
    modeler_lib
    glm
)

gtest_discover_tests(plane_tests)

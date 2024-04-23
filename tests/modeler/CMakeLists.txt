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


add_executable(
    camera_system_tests
    cameraSystemTests.cpp
)

target_link_libraries(
    camera_system_tests
    PRIVATE
    GTest::gtest_main
    modeler_lib
    ecs
    glm
)

gtest_discover_tests(camera_system_tests)

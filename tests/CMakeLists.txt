FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(
    sample_test
    samples.cpp
)

target_link_libraries(
    sample_test
    PRIVATE
    GTest::gtest_main
)

gtest_discover_tests(sample_test)


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

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


add_subdirectory(ecs/)

add_subdirectory(modeler/)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
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

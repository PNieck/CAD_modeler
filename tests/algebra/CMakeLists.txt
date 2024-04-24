add_executable(
    vector_tests
    vectorTests.cpp
)

target_link_libraries(
    vector_tests
    PRIVATE
    GTest::gtest_main
    algebra
)

gtest_discover_tests(vector_tests)


add_executable(
    matrix_tests
    matrixTests.cpp
)

target_link_libraries(
    matrix_tests
    PRIVATE
    GTest::gtest_main
    algebra
    glm
)

gtest_discover_tests(matrix_tests)

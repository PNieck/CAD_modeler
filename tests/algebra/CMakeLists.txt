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


add_executable(
    quaternion_tests
    quaternionTests.cpp
)

target_link_libraries(
    quaternion_tests
    PRIVATE
    GTest::gtest_main
    algebra
    glm
)

gtest_discover_tests(quaternion_tests)


add_executable(
    linearEquations_tests
    systemsOfLinearEquationsTests.cpp
)

target_link_libraries(
    linearEquations_tests
    PRIVATE
    GTest::gtest_main
    algebra
)

gtest_discover_tests(linearEquations_tests)


add_executable(
    cubicPolynomials_tests
    cubicPolynomialsTests.cpp
)

target_link_libraries(
    cubicPolynomials_tests
    PRIVATE
    GTest::gtest_main
    algebra
)

gtest_discover_tests(cubicPolynomials_tests)

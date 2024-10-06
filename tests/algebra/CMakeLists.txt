add_executable(
    algebra_tests
    vectorTests.cpp
    matrixTests.cpp
    quaternionTests.cpp
    systemsOfLinearEquationsTests.cpp
    cubicPolynomialsTests.cpp
    conjugateGradientMethodTests.cpp
    newtonMethodTests.cpp
)

target_link_libraries(
    algebra_tests
    PRIVATE
    GTest::gtest_main
    algebra
    glm
)

gtest_discover_tests(algebra_tests)
enable_compiler_warings(algebra_tests)

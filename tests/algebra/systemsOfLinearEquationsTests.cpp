#include <gtest/gtest.h>

#include <algebra/systemsOfLinearEquasions.hpp>


using namespace alg;

// TODO: fix test
// TEST(LinearEquationsSolvers, TridiagonalMtx) {
//     std::vector<float> superdiagonal = {1, 1, 1};
//     std::vector<float> diagonal = {2, 2, 2, 2};
//     std::vector<float> subdiagonal = {3, 3, 3};
//     std::vector<float> results = {4, 5, 6, 7};
//
//     std::vector<float> expAlgResult = {16.f/11.f, 12.f/11.f, -17.f/11.f, 64.f/11.f};
//
//     auto algResult = SolveSystemOfLinearEquationsWithTridiagonalMtx(
//         superdiagonal,
//         diagonal,
//         subdiagonal,
//         results
//     );
//
//     ASSERT_TRUE(algResult.has_value());
//     EXPECT_EQ(algResult.value().size(), expAlgResult.size());
//
//     for (int i=0; i < algResult.value().size(); ++i) {
//         EXPECT_FLOAT_EQ(algResult.value()[i], expAlgResult[i]);
//     }
// }


TEST(LinearEquationsSolvers, TridiagonalMtxWithDifferentElements) {
    std::vector<float> superdiagonal = {1, 2, 3};
    std::vector<float> diagonal = {4, 5, 6, 7};
    std::vector<float> subdiagonal = {8, 9, 10};
    std::vector<float> results = {11, 12, 13, 14};

    // std::vector<float> expAlgResult = {193.f/90.f, 109.f/45.f, -259.f/30.f, 43.f/3.f};

    auto algResult = SolveSystemOfLinearEquationsWithTridiagonalMtx(
        superdiagonal,
        diagonal,
        subdiagonal,
        results
    );

    ASSERT_FALSE(algResult.has_value());
}

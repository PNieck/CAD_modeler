#include <gtest/gtest.h>

#include <algebra/cubicPolynomials.hpp>



using namespace alg;


TEST(CubicPolynomialsTests, SwitchingFromPowerBasisToBernstein) {
    Vec4 polynomialInPowerBase(1.f, 1.f, 1.f, 1.f);

    Vec4 expPolynomial(1.f, 4.f/3.f, 2.f, 4.f );

    auto polynomialInBernsteinBase = FromPowerToBernsteinBasis(polynomialInPowerBase);

    ASSERT_EQ(polynomialInBernsteinBase, expPolynomial);
}

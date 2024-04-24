#include <gtest/gtest.h>

#include <algebra/vec3.hpp>

using namespace alg;


TEST(VectorTests, ConstructionOfVec3) {
    Vec3 v(1.0f, 2.0f, 3.0f);

    ASSERT_EQ(v.X(), 1.0f);
    ASSERT_EQ(v.Y(), 2.0f);
    ASSERT_EQ(v.Z(), 3.0f);
}

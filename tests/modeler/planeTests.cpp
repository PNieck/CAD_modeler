#include <gtest/gtest.h>

#include <CAD_modeler/utilities/plane.hpp>


TEST(PlaneLineIntersectionTests, LineStartPosIsIntersectionPoint) {
    alg::Vec3 intersectionPoint(3, 5, 7);
    alg::Vec3 planePerpendicular(0, 0, 1);
    alg::Vec3 lineDirection(0, 0, 1);

    Plane plane(intersectionPoint, planePerpendicular);
    Line line(intersectionPoint, lineDirection);

    std::optional<alg::Vec3> intersection = plane.Intersect(line);

    ASSERT_TRUE(intersection.has_value());
    ASSERT_EQ(intersection.value(), intersectionPoint);
}

#include <iostream>
TEST(PlaneLineIntersectionTests, LineStartPosIsNotNIntersectionPoint) {
    alg::Vec3 intersectionPoint(3, 5, 7);
    alg::Vec3 planePerpendicular(0, 0, 1);
    alg::Vec3 lineDirection(0, 0, 1);

    Plane plane(intersectionPoint, planePerpendicular);
    Line line(intersectionPoint + 5.0f*lineDirection, lineDirection);

    std::optional<alg::Vec3> intersection = plane.Intersect(line);

    std::cout << "\n" << intersection.value().X() << " "
        << intersection.value().Y() << " "
        << intersection.value().Z() << " \n\n";

    ASSERT_TRUE(intersection.has_value());
    ASSERT_EQ(intersection.value(), intersectionPoint);
}

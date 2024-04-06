#include <gtest/gtest.h>

#include <CAD_modeler/utilities/plane.hpp>


TEST(PlaneLineIntersectionTests, LineStartPosIsIntersectionPoint) {
    glm::vec3 intersectionPoint(3, 5, 7);
    glm::vec3 planePerpendicular(0, 0, 1);
    glm::vec3 lineDirection(0, 0, 1);

    Plane plane(intersectionPoint, planePerpendicular);
    Line line(intersectionPoint, lineDirection);

    std::optional<glm::vec3> intersection = plane.Intersect(line);

    ASSERT_TRUE(intersection.has_value());
    ASSERT_EQ(intersection.value(), intersectionPoint);
}


TEST(PlaneLineIntersectionTests, LineStartPosIsNotNIntersectionPoint) {
    glm::vec3 intersectionPoint(3, 5, 7);
    glm::vec3 planePerpendicular(0, 0, 1);
    glm::vec3 lineDirection(0, 0, 1);

    Plane plane(intersectionPoint, planePerpendicular);
    Line line(intersectionPoint + 5.0f*lineDirection, lineDirection);

    std::optional<glm::vec3> intersection = plane.Intersect(line);

    ASSERT_TRUE(intersection.has_value());
    ASSERT_EQ(intersection.value(), intersectionPoint);
}

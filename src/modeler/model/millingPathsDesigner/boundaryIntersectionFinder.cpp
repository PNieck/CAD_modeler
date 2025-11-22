#include <CAD_modeler/model/millingPathsDesigner/boundaryIntersectionFinder.hpp>


BoundaryIntersectionFinder::BoundaryIntersectionFinder(const std::vector<Position> &boundary):
    boundary(boundary)
{
}


Position BoundaryIntersectionFinder::Intersection(const LineSegment2D &segment)
{
    actIdx++;

    while (true) {
        LineSegment2D boundarySeg = ActSegment();

        if (LineSegment2D::AreIntersecting(boundarySeg, segment)) {
            auto interPoint = LineSegment2D::IntersectionPoint(boundarySeg, segment).value();
            //prevIntersectionPoint = interPoint;

            return { interPoint.X(), boundary[actIdx].GetY(), interPoint.Y() };
        }

        actIdx++;
    }
}


Position BoundaryIntersectionFinder::Intersection(const float xValue)
{
    const LineSegment2D segment(xValue, 2.f, xValue, -2.f);
    return Intersection(segment);
}


// Position BoundaryIntersectionFinder::GoAlongBoundary(float dist)
// {
//     float actDist = 0.f;
//
//     LineSegment2D actSeg(
//         prevIntersectionPoint.X(),
//         prevIntersectionPoint.Y(),
//         boundary[actIdx].GetX(),
//         boundary[actIdx].GetZ()
//     );
//
//     while (actDist < dist) {
//         actDist += actSeg.Length();
//
//         actIdx++;
//     }
// }


LineSegment2D BoundaryIntersectionFinder::ActSegment() const
{
    return LineSegment2D (
            boundary[actIdx-1].GetX(),
            boundary[actIdx-1].GetZ(),
            boundary[actIdx].GetX(),
            boundary[actIdx].GetZ()
        );
}

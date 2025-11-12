#include <CAD_modeler/model/components/millingCutter.hpp>


float MillingCutter::YCoordinate(const Position &pos, const float x, const float z) const
{
    const float diffX = x - pos.GetX();
    const float diffZ = z - pos.GetZ();

    const float lenSq = diffX*diffX + diffZ*diffZ;

    const float radiusSq = radius * radius;
    if (lenSq > radiusSq)
        return std::numeric_limits<float>::infinity();

    switch (type) {
        case Type::Flat:
            return pos.GetY();

        case Type::Round:
            return radius - std::sqrt(radiusSq - lenSq) + pos.GetY();

        default:
            throw std::runtime_error("Unknown cutter type");
    }
}


float MillingCutter::CalcMinYCoord(const float cutterX, const float cutterZ, const Position &pointPos) const
{
    const float diffX = cutterX - pointPos.GetX();
    const float diffZ = cutterZ - pointPos.GetZ();

    const float lenSq = diffX*diffX + diffZ*diffZ;
    const float radiusSq = radius * radius;

    if (lenSq > radiusSq)
        return -std::numeric_limits<float>::infinity();

    switch (type) {
        case Type::Flat:
            return pointPos.GetY();

        case Type::Round:
            return pointPos.GetY() - radius + std::sqrt(radiusSq - lenSq);

        default:
            throw std::runtime_error("Unknown cutter type");
    }
}

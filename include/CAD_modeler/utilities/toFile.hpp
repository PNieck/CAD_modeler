#pragma once

#include <algebra/vec2.hpp>

#include "CAD_modeler/model/components/intersectionCurve.hpp"

#include <fstream>
#include <string>
#include <vector>


inline void InterCurveToFile(const std::string& fileName, const IntersectionCurve& curve) {
    std::ofstream file(fileName);
    for (const auto& p: curve)
        file << p.U1() << ", " << p.V1() << std::endl;
}


inline void InterCurveToFile(const std::string& fileName, const std::vector<alg::Vec2>& curve) {
    std::ofstream file(fileName);
    for (const auto& p: curve)
        file << p.X() << ", " << p.Y() << std::endl;
}

#pragma once

#include <algebra/vec2.hpp>

#include "CAD_modeler/model/components/intersectionCurve.hpp"

#include <fstream>
#include <string_view>
#include <vector>


inline void InterCurveToFile(const std::string_view fileName, const IntersectionCurve& curve) {
    std::ofstream file(fileName.data());
    for (const auto& p: curve)
        file << p.U1() << ", " << p.V1() << std::endl;
}


inline void InterCurveToFile(const std::string_view fileName, const std::vector<alg::Vec2>& curve) {
    std::ofstream file(fileName.data());
    for (const auto& p: curve)
        file << p.X() << ", " << p.Y() << std::endl;
}


template<typename T>
void Vector2DToCSV(const std::string_view fileName, const FlatVec2D<T>& vec) {
    std::ofstream file(fileName.data());
    for (size_t row = 0; row < vec.Rows(); ++row) {
        for (size_t col = 0; col < vec.Cols() - 1; ++col) {
            file << vec.At(row, col) << ", ";
        }

        file << vec.At(row, vec.Cols()-1) << std::endl;
    }
}

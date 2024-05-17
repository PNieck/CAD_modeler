#pragma once

#include "vec4.hpp"


namespace alg
{
    template <typename DataType>
    alg::Vector4<DataType> FromPowerToBernsteinBasis(const alg::Vector4<DataType>& polynomial) {
        return alg::Vector4<DataType>(
            polynomial.X(),
            polynomial.X() + (1.f/3.f) * polynomial.Y(),
            polynomial.X() + (2.f/3.f) * polynomial.Y() + (1.f/3.f) * polynomial.Z(),
            polynomial.X() + polynomial.Y() + polynomial.Z() + polynomial.W()
        );
    }
}

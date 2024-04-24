#pragma once

#include "vec3.hpp"


namespace alg
{
    class Pos3 {
    public:
        Pos3(float x, float y, float z):
            coordinates(x, y, z) {}

        inline float& X()
            { return coordinates.X(); }

        inline float X() const
            { return coordinates.X(); }

        inline float& Y()
            { return coordinates.X(); }

        inline float Y() const
            { return coordinates.X(); }

        inline float& Z()
            { return coordinates.X(); }

        inline float Z() const
            { return coordinates.X(); }

    private:
        Vec3 coordinates;    
    };
};

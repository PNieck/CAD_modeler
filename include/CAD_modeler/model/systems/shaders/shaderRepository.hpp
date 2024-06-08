#pragma once

#include "stdShader.hpp"
#include "gridShader.hpp"
#include "cubicBezierCurveShader.hpp"
#include "bicubicBezierSurfaceShader.hpp"


class ShaderRepository {
public:
    inline const StdShader& GetStdShader() const
        { return stdShader; }

    inline const GridShader& GetGridShader() const
        { return gridShader; }

    inline const CubicBezierCurveShader& GetBezierCurveShader() const
        { return bezierCurveShader; }

    inline const BicubicBezierSurfaceShader& GetBezierSurfaceShader() const
        { return bezierSurfaceShader; }

private:
    StdShader stdShader;
    GridShader gridShader;
    CubicBezierCurveShader bezierCurveShader;
    BicubicBezierSurfaceShader bezierSurfaceShader;
};

#pragma once

#include "stdShader.hpp"
#include "gridShader.hpp"
#include "cubicBezierShader.hpp"


class ShaderRepository {
public:
    inline const StdShader& GetStdShader() const
        { return stdShader; }

    inline const GridShader& GetGridShader() const
        { return gridShader; }

    inline const CubicBezierCurveShader& GetBezierCurveShader() const
        { return bezierShader; }

private:
    StdShader stdShader;
    GridShader gridShader;
    CubicBezierCurveShader bezierShader;
};

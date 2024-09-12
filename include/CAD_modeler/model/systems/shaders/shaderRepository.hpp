#pragma once

#include "stdShader.hpp"
#include "gridShader.hpp"
#include "cubicBezierCurveShader.hpp"
#include "bicubicBezierSurfaceShader.hpp"
#include "bicubicBSplineSurfaceShader.hpp"
#include "gregoryPatchShader.hpp"
#include "passThrough.hpp"


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

    inline const BicubicBSplineSurfaceShader& GetBSplineSurfaceShader() const
        { return bSplineSurfaceShader; }

    inline const GregoryPatchShader& GetGregoryPatchShader() const
        { return gregoryPatchShader; }

    inline const PassThroughShader& GetPassThroughShader() const
        { return passThroughShader; }

private:
    StdShader stdShader;
    GridShader gridShader;
    CubicBezierCurveShader bezierCurveShader;
    BicubicBezierSurfaceShader bezierSurfaceShader;
    BicubicBSplineSurfaceShader bSplineSurfaceShader;
    GregoryPatchShader gregoryPatchShader;
    PassThroughShader passThroughShader;
};

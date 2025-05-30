#pragma once

#include "stdShader.hpp"
#include "gridShader.hpp"
#include "cubicBezierCurveShader.hpp"
#include "bicubicBezierSurfaceShader.hpp"
#include "bicubicBSplineSurfaceShader.hpp"
#include "gregoryPatchShader.hpp"
#include "passThrough.hpp"
#include "millingShader.hpp"
#include "trimmedTorusShader.hpp"


class ShaderRepository {
public:
    static ShaderRepository& GetInstance() {
        static ShaderRepository instance;
        return instance;
    }

    [[nodiscard]]
    const StdShader& GetStdShader() const
        { return stdShader; }

    [[nodiscard]]
    const GridShader& GetGridShader() const
        { return gridShader; }

    [[nodiscard]]
    const CubicBezierCurveShader& GetBezierCurveShader() const
        { return bezierCurveShader; }

    [[nodiscard]]
    const BicubicBezierSurfaceShader& GetBezierSurfaceShader() const
        { return bezierSurfaceShader; }

    [[nodiscard]]
    const BicubicBSplineSurfaceShader& GetBSplineSurfaceShader() const
        { return bSplineSurfaceShader; }

    [[nodiscard]]
    const GregoryPatchShader& GetGregoryPatchShader() const
        { return gregoryPatchShader; }

    [[nodiscard]]
    const PassThroughShader& GetPassThroughShader() const
        { return passThroughShader; }

    [[nodiscard]]
    const MillingShader& GetMillingShader() const
        { return millingShader; }

    const TrimmedTorusShader& GetTrimmedTorusShader() const
        { return trimmedTorusShader; }

    ShaderRepository(const ShaderRepository&) = delete;
    ShaderRepository(ShaderRepository&&) = delete;
    ShaderRepository& operator=(const ShaderRepository&) = delete;

private:
    StdShader stdShader;
    GridShader gridShader;
    CubicBezierCurveShader bezierCurveShader;
    BicubicBezierSurfaceShader bezierSurfaceShader;
    BicubicBSplineSurfaceShader bSplineSurfaceShader;
    GregoryPatchShader gregoryPatchShader;
    PassThroughShader passThroughShader;
    MillingShader millingShader;
    TrimmedTorusShader trimmedTorusShader;

    ShaderRepository() = default;
};

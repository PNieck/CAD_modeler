#pragma once

#include "stdShader.hpp"
#include "gridShader.hpp"
#include "cubicBezierCurveShader.hpp"
#include "bicubicBezierSurfaceShader.hpp"
#include "bicubicBSplineSurfaceShader.hpp"
#include "gregoryPatchShader.hpp"
#include "passThrough.hpp"
#include "millingShader.hpp"


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

private:
    StdShader stdShader;
    GridShader gridShader;
    CubicBezierCurveShader bezierCurveShader;
    BicubicBezierSurfaceShader bezierSurfaceShader;
    BicubicBSplineSurfaceShader bSplineSurfaceShader;
    GregoryPatchShader gregoryPatchShader;
    PassThroughShader passThroughShader;
    MillingShader millingShader;

    ShaderRepository() = default;
    ShaderRepository(const ShaderRepository&) = delete;
    ShaderRepository(ShaderRepository&&) = delete;
    ShaderRepository& operator=(const ShaderRepository&) = delete;
};

#pragma once

#include "stdShader.hpp"
#include "gridShader.hpp"
#include "cubicBezierCurveShader.hpp"
#include "bicubicBezierSurfaceShader.hpp"
#include "bicubicBSplineSurfaceShader.hpp"
#include "gregoryPatchShader.hpp"
#include "passThrough.hpp"
#include "millingMaterialTopShader.hpp"
#include "millingMaterialBottomShader.hpp"
#include "millingMaterialSideShader.hpp"
#include "trimmedTorusShader.hpp"
#include "trimmedBicubicBezierSurfaceShader.hpp"
#include "trimmedBicubicBSplineSurfaceShader.hpp"


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
    const MillingMaterialTopShader& GetMillingMaterialTopShader() const
        { return millingShader; }

    [[nodiscard]]
    const MillingMaterialBottomShader& GetMillingMaterialBottomShader() const
        { return millingBottomShader; }

    [[nodiscard]]
    const MillingMaterialSideShader& GetMillingMaterialSideShader() const
        { return millingSideShader; }

    [[nodiscard]]
    const TrimmedTorusShader& GetTrimmedTorusShader() const
        { return trimmedTorusShader; }

    [[nodiscard]]
    const TrimmedBicubicBezierSurfaceShader& GetTrimmedBezierSurfaceShader() const
        { return trimmedBicubicBezierSurfaceShader; }

    [[nodiscard]]
    const TrimmedBicubicBSplineSurfaceShader& GetTrimmedBSplineSurfaceShader() const
        { return trimmedBicubicBSplineSurfaceShader; }

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
    MillingMaterialTopShader millingShader;
    MillingMaterialBottomShader millingBottomShader;
    MillingMaterialSideShader millingSideShader;
    TrimmedTorusShader trimmedTorusShader;
    TrimmedBicubicBezierSurfaceShader trimmedBicubicBezierSurfaceShader;
    TrimmedBicubicBSplineSurfaceShader trimmedBicubicBSplineSurfaceShader;

    ShaderRepository() = default;
};

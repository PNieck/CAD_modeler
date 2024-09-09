#pragma once

#include "position.hpp"


class GregoryPatchParameters {
public:
    enum OuterPointSpec {
        // First row
        _00 = 0,
        _01,
        _02,
        _03,

        // Second row
        _10,
        _13,

        // Third row
        _20,
        _23,
        
        // Fourth row
        _30,
        _31,
        _32,
        _33
    };

    enum InnerPointsSpec {
        neighbourOf01 = 0,
        neighbourOf02,

        neighbourOf10,
        neighbourOf13,

        neighbourOf20,
        neighbourOf23,

        neighbourOf31,
        neighbourOf32
    };

    static constexpr int OuterPointsNb = 12;
    static constexpr int InnerPointsNb = 8;

    Position GetOuterPoint(int row, int col) const;
    inline Position GetOuterPoint(OuterPointSpec spec) const
        { return outerPoints[spec]; }
    
    void SetOuterPoint(const Position& pos, int row, int col);
    inline void SetOuterPoint(const Position& pos, OuterPointSpec spec)
        { outerPoints[spec] = pos; }

    Position GetInnerPoint(int neighbourRow, int neighbourCol) const;
    inline Position GetInnerPoint(InnerPointsSpec spec) const
        { return innerPoints[spec]; }

    void SetInnerPoint(const Position& pos, int neighbourRow, int neighbourCol);
    inline void SetInnerPoint(const Position& pos, InnerPointsSpec spec)
        { innerPoints[spec] = pos; }

private:
    Position outerPoints[OuterPointsNb];
    Position innerPoints[InnerPointsNb];
};


struct TriangleOfGregoryPatches {
    static constexpr int ParamsCnt = 3;

    GregoryPatchParameters patch[ParamsCnt];
};

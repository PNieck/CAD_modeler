#pragma once

#include "curveControlPoints.hpp"


class BezierControlPoints: public CurveControlPoints {
public:
    BezierControlPoints():
        CurveControlPoints({}) {}
};

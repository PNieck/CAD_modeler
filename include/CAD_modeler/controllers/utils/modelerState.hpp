#pragma once


enum class ModelerState {
    Default,

    Adding3dPoints,
    AddingC0Curve,
    AddingC2Curve,
    AddingInterpolationCurve,
    AddingIntersectionCurve,
    AddingC0Surface,
    AddingC2Surface,
    AddingC0Cylinder,
    AddingC2Cylinder,
    AddingGregoryPatches,

    AnaglyphsSettings
};

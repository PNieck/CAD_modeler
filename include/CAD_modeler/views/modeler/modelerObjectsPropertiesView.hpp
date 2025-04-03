#pragma once

#include "../../model/modeler.hpp"
#include "../../model/components/c0CurveParameters.hpp"
#include "../../model/components/c2CurveParameters.hpp"


class ModelerController;


class ModelerObjectsPropertiesView {
public:
    ModelerObjectsPropertiesView(ModelerController& controller, Modeler& model);

    void Render();

private:
    ModelerController& controller;
    Modeler& model;

    void RenderSingleObjectProperties(Entity entity) const;
    void RenderMultipleObjectProperties() const;
    void RenderMergingControlPointsOptionButton(Entity e1, Entity e2) const;

    void DisplayPositionProperty(Entity entity, const Position& pos) const;
    void DisplayScaleProperty(Entity entity, const Scale& scale) const;
    void DisplayRotationProperty(Entity entity, const Rotation& rotation) const;
    void DisplayTorusProperty(Entity entity, const TorusParameters& params) const;
    void DisplayCurveControlPoints(Entity entity, const CurveControlPoints& controlPoints) const;
    void DisplayC0CurveParameters(Entity entity, const C0CurveParameters& params) const;
    void DisplayC2CurveParameters(Entity entity, const C2CurveParameters& params) const;
    void DisplayEntityDeletionOption(Entity entity) const;
    void DisplaySurfaceDensityParameter(Entity entity, const PatchesDensity& density) const;
    void DisplaySurfacePatches(Entity entity, const Patches& patches) const;
    void DisplayGregoryPatchesParameters(Entity entity, const TriangleOfGregoryPatches& triangle) const;

    void DisplayNameEditor(Entity entity, const Name& name) const;
};

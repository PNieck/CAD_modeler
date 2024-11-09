#pragma once

#include "../model/modeler.hpp"
#include "mainMenuBar.hpp"

#include "objectTypes/curvesTypes.hpp"
#include "objectTypes/cylinderTypes.hpp"
#include "objectTypes/surfacesTypes.hpp"


class ModelerController;


class ModelerGuiView {
public:
    ModelerGuiView(ModelerController& controller, Modeler& model);

    void RenderGui();

private:
    ModelerController& controller;
    Modeler& model;

    MainMenuBar menuBar;

    static constexpr float DRAG_FLOAT_SPEED = 0.01f;
    static constexpr float DRAG_ANGLE_SPEED = 0.5f;
    static constexpr float MIN_SCALE = 0.01f;

    void RenderDefaultGui() const;
    void RenderAdd3DPointsGui();

    void RenderAddingCurveGui(CurveType curveType);
    void RenderAddingSurface(SurfaceType surfaceType);
    void RenderAddingCylinder(CylinderType CylinderType);
    void RenderAddingGregoryPatches();
    void RenderAddingIntersectionCurve();

    void RenderAnaglyphsCameraSettings();

    void RenderObjectsNames() const;
    void RenderObjectsProperties() const;

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
    

    // Utils
    void RenderSelectableEntitiesList(const std::unordered_set<Entity>& entities) const;

    [[nodiscard]]
    Entity AddCurve(const std::vector<Entity>& entities, CurveType curveType) const;

    [[nodiscard]]
    Entity AddSurface(SurfaceType surfaceType, const alg::Vec3& dir, float length, float width) const;

    [[nodiscard]]
    Entity AddCylinder(CylinderType cylinderType) const;

    void AddRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void AddColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void DeleteRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void DeleteColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    [[nodiscard]]
    int GetSurfaceRowsCnt(Entity surface, SurfaceType surfaceType) const;

    [[nodiscard]]
    int GetSurfaceColsCnt(Entity surface, SurfaceType surfaceType) const;

    void AddRowOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3& dir, float radius);

    void AddColOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3& dir, float radius);

    void DeleteRowOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3& dir, float radius);

    void DeleteColOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3& dir, float radius);

    [[nodiscard]]
    int GetCylinderRowsCnt(Entity cylinder, CylinderType CylinderType) const;

    [[nodiscard]]
    int GetCylinderColsCnt(Entity cylinder, CylinderType CylinderType) const;

    void RecalculateCylinder(Entity cylinder, CylinderType cylinderType, const alg::Vec3& dir, float radius);

    void RecalculateSurface(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void AddControlPointToCurve(Entity curve, Entity entity, CurveType curveType) const;
};

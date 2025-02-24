#pragma once

#include "../../model/modeler.hpp"

#include "../objectTypes/curvesTypes.hpp"
#include "../objectTypes/cylinderTypes.hpp"
#include "../objectTypes/surfacesTypes.hpp"


class ModelerController;


class ModelerMainMenuView {
public:
    ModelerMainMenuView(ModelerController& controller, Modeler& model);

    void Render();

private:
    ModelerController& controller;
    Modeler& model;

    void RenderDefaultGui() const;

    void RenderAdd3DPointsGui();
    void RenderAddingCurveGui(CurveType curveType);
    void RenderAddingSurface(SurfaceType surfaceType);
    void RenderAddingCylinder(CylinderType CylinderType);
    void RenderAddingGregoryPatches();
    void RenderAddingIntersectionCurve();

    void RenderObjectsNames() const;

    void RenderAnaglyphsCameraSettings();


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
};

#pragma once

#include "../controllers/guiController.hpp"
#include "../modeler.hpp"
#include "mainMenuBar.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


class GuiView {
public:
    GuiView(GLFWwindow* window, GuiController& controller, const Modeler& model);
    ~GuiView();

    void RenderGui();

private:
    GuiController& controller;
    const Modeler& model;

    MainMenuBar menuBar;

    static constexpr float DRAG_FLOAT_SPEED = 0.01f;
    static constexpr float DRAG_ANGLE_SPEED = 0.5f;
    static constexpr float MIN_SCALE = 0.01f;

    void RenderDefaultGui() const;
    void RenderAdd3DPointsGui() const;

    void RenderAddingCurveGui(CurveType curveType) const;
    void RenderAddingSurface(SurfaceType surfaceType) const;
    void RenderAddingCylinder(CylinderType CylinderType) const;
    void RenderAddingGregoryPatches() const;
    void RenderAddingIntersectionCurve() const;

    void RenderAnaglyphsCameraSettings() const;

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

};

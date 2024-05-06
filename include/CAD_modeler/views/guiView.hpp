#pragma once

#include "../controllers/guiController.hpp"
#include "../model.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


class GuiView {
public:
    GuiView(GLFWwindow* window, GuiController& controller, const Model& model);
    ~GuiView();

    void RenderGui() const;

private:
    GuiController& controller;
    const Model& model;

    static constexpr float DRAG_FLOAT_SPEED = 0.01f;
    static constexpr float DRAG_ANGLE_SPEED = 0.5f;
    static constexpr float MIN_SCALE = 0.01f;

    void RenderDefaultGui() const;
    void RenderAdd3DPointsGui() const;

    void RenderAddingCurveGui(CurveType curveType) const;

    void RenderObjectsNames() const;
    void RenderObjectsProperties() const;

    void RenderSingleObjectProperties(Entity entity) const;
    void RenderMultipleObjectProperties() const;

    void DisplayPositionProperty(Entity entity, const Position& pos) const;
    void DisplayScaleProperty(Entity entity, const Scale& scale) const;
    void DisplayRotationProperty(Entity entity, const Rotation& rotation) const;
    void DisplayTorusProperty(Entity entity, const TorusParameters& params) const;
    void DisplayCurveControlPoints(Entity entity, const CurveControlPoints& controlPoints) const;
    void DisplayC0CurveParameters(Entity entity, const C0CurveParameters& params) const;
    void DisplayC2CurveParameters(Entity entity, const C2CurveParameters& params) const;
    void DisplayEntityDeletionOption(Entity entity) const;

    void DisplayNameEditor(Entity entity, const Name& name) const;
    
};

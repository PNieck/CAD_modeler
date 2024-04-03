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

    void RenderDefaultGui() const;
    void RenderAdd3DPointsGui() const;

    void RenderObjectsNames() const;
    void RenderObjectsProperties() const;

    void RenderSingleObjectProperties(Entity entity) const;

    void DisplayPositionProperty(Entity entity, const Position& pos) const;
};

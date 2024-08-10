#pragma once

#include "../controllers/guiController.hpp"
#include "../model.hpp"


class MainMenuBar {
public:
    MainMenuBar(GuiController& controller, const Model& model):
        controller(controller), model(model) {}

    void Render() const;

private:
    GuiController& controller;
    const Model& model;
};

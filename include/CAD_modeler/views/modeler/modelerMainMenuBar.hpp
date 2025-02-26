#pragma once

#include "../../model/modeler.hpp"


class ModelerController;


class ModelerMainMenuBar {
public:
    ModelerMainMenuBar(ModelerController& controller, Modeler& model);

    void Render();

private:
    ModelerController& controller;
    Modeler& model;
};

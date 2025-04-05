#pragma once

#include "../../model/modeler.hpp"


class ModelerController;


class ModelerMenuBar {
public:
    ModelerMenuBar(ModelerController& controller, Modeler& model);

    void Render();

private:
    ModelerController& controller;
    Modeler& model;
};

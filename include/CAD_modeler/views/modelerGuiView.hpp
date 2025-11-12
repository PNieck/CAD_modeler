#pragma once

#include "../model/modeler.hpp"
#include "modeler/modelerMenuBar.hpp"
#include "modeler/modelerMainMenuView.hpp"
#include "modeler/modelerObjectsPropertiesView.hpp"


class ModelerController;


class ModelerGuiView {
public:
    ModelerGuiView(ModelerController& controller, Modeler& model);

    void Render();

private:
    ModelerMenuBar menuBar;
    ModelerMainMenuView mainMenu;
    ModelerObjectsPropertiesView propertiesView;
};

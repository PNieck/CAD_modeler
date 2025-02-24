#pragma once

#include "../../model/modeler.hpp"
#include "modelerMainMenuBar.hpp"
#include "modelerMainMenuView.hpp"
#include "modelerObjectsPropertiesView.hpp"


class ModelerController;


class ModelerGuiView {
public:
    ModelerGuiView(ModelerController& controller, Modeler& model);

    void Render();

private:
    ModelerMainMenuBar menuBar;
    ModelerMainMenuView mainMenu;
    ModelerObjectsPropertiesView propertiesView;
};

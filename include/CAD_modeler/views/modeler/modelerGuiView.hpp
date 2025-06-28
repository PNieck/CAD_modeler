#pragma once

#include "../../model/modeler.hpp"
#include "modelerMenuBar.hpp"
#include "modelerMainMenuView.hpp"
#include "modelerObjectsPropertiesView.hpp"


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

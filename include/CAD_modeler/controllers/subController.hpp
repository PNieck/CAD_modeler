#pragma once

#include "utils/appState.hpp"
#include "../model/modeler.hpp"


class MainController;


class SubController {
public:
    SubController(Modeler& model, MainController& controller):
        model(model),  mainController(controller) {}


    AppState GetAppState() const;
    void SetAppState(AppState newState) const;

protected:
    Modeler& model;

private:
    MainController& mainController;
};

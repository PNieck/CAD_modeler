#pragma once

#include "utils/appState.hpp"
#include "../model.hpp"


class MainController;


class SubController {
public:
    SubController(Model& model, MainController& controller):
        model(model),  mainController(controller) {}


    AppState GetAppState() const;
    void SetAppState(AppState newState) const;

protected:
    Model& model;

private:
    MainController& mainController;
};

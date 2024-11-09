#pragma once

#include "CAD_modeler/controllers/utils/modelTypes.hpp"


class MainController;


class ModelChooser {
public:
    explicit ModelChooser(MainController& controller):
        controller(controller) {}

    void Render() const;

private:
    MainController& controller;

    [[nodiscard]]
    int ModelToIndex() const;

    static ModelType IndexToModelType(int index);
};

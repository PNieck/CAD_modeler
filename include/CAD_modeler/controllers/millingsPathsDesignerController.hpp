#pragma once

#include "subController.hpp"
#include "../model/millingPathsDesigner.hpp"
#include "../views/millingPathsDesignerView.hpp"


class MillingsPathsDesignerController: public SubController {
public:
    explicit MillingsPathsDesignerController(MillingPathsDesigner& model);

    void Render() override;

    void Update(double dt) override
        { model.Update(); }

private:
    MillingPathsDesigner model;
    MillingPathsDesignerView view;
};

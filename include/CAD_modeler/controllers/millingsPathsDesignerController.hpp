#pragma once

#include "subController.hpp"
#include "../model/millingPathsDesigner.hpp"


class MillingsPathsDesignerController: public SubController {
public:
    explicit MillingsPathsDesignerController(MillingPathsDesigner& model);

    void Render() override;

    void Update(double dt) override {}

private:
    MillingPathsDesigner model;
};

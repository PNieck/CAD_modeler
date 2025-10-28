#pragma once

#include <CAD_modeler/model/millingPathsDesigner.hpp>


class MillingPathsDesignerView {
public:
    explicit MillingPathsDesignerView(MillingPathsDesigner& model);

    void RenderGui();

private:
    std::string filePath;

    MillingPathsDesigner& model;

    void RenderFileSelection();
};

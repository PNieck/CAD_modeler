#include <CAD_modeler/views/modelChooser.hpp>

#include <CAD_modeler/controllers/mainController.hpp>

#include <imgui.h>

#include <stdexcept>


void ModelChooser::Render() const
{
    static const char* items[] = {
        "Modeling",
        "Milling simulation",
        "Millings Paths Designer"
    };

    ImGui::Begin("Choose mode");
    int actIndex = ModelToIndex();

    if (ImGui::Combo("Change mode", &actIndex, items, IM_ARRAYSIZE(items))) {
        controller.SetModelType(IndexToModelType(actIndex));
    }

    ImGui::End();
}


int ModelChooser::ModelToIndex() const
{
    switch (controller.GetModelType()) {
        case ModelType::Modeler:
            return 0;

        case ModelType::MillerMachineSim:
            return 1;

        case ModelType::MillingPathsDesigner:
            return 2;

        default:
            throw std::runtime_error("Unknown model type");
    }
}


ModelType ModelChooser::IndexToModelType(const int index)
{
    switch (index) {
        case 0:
            return ModelType::Modeler;

        case 1:
            return ModelType::MillerMachineSim;

        case 2:
            return ModelType::MillingPathsDesigner;

        default:
            throw std::runtime_error("Unknown model type");
    }
}

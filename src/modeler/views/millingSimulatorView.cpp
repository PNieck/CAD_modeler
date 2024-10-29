#include <CAD_modeler/views/millingSimulatorView.hpp>

#include <imgui.h>


MillingSimulatorView::MillingSimulatorView(MillingSimController &controller, MillingMachineSim &model):
    controller(controller), model(model)
{
    fileBrowser.SetTitle("Choose GCode file");
}


void MillingSimulatorView::RenderGui()
{
    ImGui::Begin("Milling simulator");

    RenderFileSelection();
    RenderMaterialOptions();
    RenderSimulationOptions();
    RenderCutterInformation();

    ImGui::End();
}


void MillingSimulatorView::RenderFileSelection()
{
    ImGui::SeparatorText("File");

    if (filePath.empty())
        ImGui::Text("No file selected");
    else
        ImGui::Text("Selected file: %s", filePath.c_str());

    if (ImGui::Button("Load"))
        fileBrowser.Open();

    fileBrowser.Display();

    if (fileBrowser.HasSelected()) {
        filePath = fileBrowser.GetSelected().string();
        fileBrowser.ClearSelected();
        fileBrowser.Close();

        model.AddMillingPath(filePath);
    }
}


void MillingSimulatorView::RenderMaterialOptions() const
{
    ImGui::SeparatorText("Material");

    bool valuesChanged = false;
    int xResolution = model.GetMaterialXResolution();
    int zResolution = model.GetMaterialZResolution();

    ImGui::BeginDisabled(model.MillingMachineRuns());

    valuesChanged |= ImGui::InputInt("X resolution", &xResolution);
    valuesChanged |= ImGui::InputInt("Z resolution", &zResolution);

    if (valuesChanged) {
        model.SetMaterialResolution(xResolution, zResolution);
        valuesChanged = false;
    }

    float materialXLen = model.GetMaterialXLength();
    float materialZLen = model.GetMaterialZLength();
    valuesChanged |= ImGui::DragFloat("Material size X", &materialXLen, 0.1f);
    valuesChanged |= ImGui::DragFloat("Material size Z", &materialZLen, 0.1f);

    if (valuesChanged) {
        model.SetMaterialLength(materialXLen, materialZLen);
    }

    float thickness = model.GetMaterialThickness();
    if (ImGui::DragFloat("Material thickness", &thickness))
        model.SetMaterialThickness(thickness);

    float base = model.GetMaterialBaseLevel();
    if (ImGui::DragFloat("Material base level", &base))
        model.SetMaterialBaseLevel(base);

    if (ImGui::Button("Reset material"))
        model.ResetMaterial();

    ImGui::EndDisabled();
}


void MillingSimulatorView::RenderSimulationOptions() const
{
    ImGui::SeparatorText("Simulation");

    const bool simulationRuns = model.MillingMachineRuns();

    ImGui::BeginDisabled(simulationRuns);

    float cutterSpeed = model.GetCutterSpeed();
    if (ImGui::DragFloat("Cutter speed", &cutterSpeed))
        model.SetCutterSpeed(cutterSpeed);

    if (ImGui::Button("Start"))
        model.StartSimulation();

    ImGui::EndDisabled();

    ImGui::BeginDisabled(!simulationRuns);
    if (ImGui::Button("Stop"))
        model.StopSimulation();

    ImGui::EndDisabled();
}


void MillingSimulatorView::RenderCutterInformation() const
{
    const auto cutter = model.GetMillingCutter();

    ImGui::SeparatorText("Cutter");

    if (!cutter.has_value()) {
        ImGui::Text("No cutter");
        return;
    }

    ImGui::Text("Radius: %f mm", cutter.value().radius * 100.f);

    switch (cutter.value().type) {
        case MillingCutter::Type::Flat:
            ImGui::Text("Type: Flat");
            break;

        case MillingCutter::Type::Round:
            ImGui::Text("Type: Round");
            break;

        default:
            throw std::runtime_error("Unknown cutter type");
    }
}




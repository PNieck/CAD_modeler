#include <CAD_modeler/views/millingSimulatorView.hpp>

#include <imgui.h>
#include <ImGuiFileDialog.h>


MillingSimulatorView::MillingSimulatorView(MillingSimController &controller, MillingMachineSim &model):
    controller(controller), model(model)
{
}


void MillingSimulatorView::RenderGui()
{
    ImGui::Begin("Milling simulator");

    RenderFileSelection();
    RenderMaterialOptions();
    RenderSimulationOptions();
    RenderCutterInformation();
    RenderWarnings();

    ImGui::End();
}


void MillingSimulatorView::RenderFileSelection()
{
    ImGui::SeparatorText("File");

    if (filePath.empty())
        ImGui::Text("No file selected");
    else
        ImGui::Text("Selected file: %s", filePath.c_str());

    if (ImGui::Button("Load")) {
        IGFD::FileDialogConfig config;
	    config.path = ".";

        ImGuiFileDialog::Instance()->OpenDialog(
            "ChooseGCodeFileDlgKey",
            "Choose GCode File",
            "GCode files {.k01,.k08,.k16,.f10,.f12}",
            config
        );
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseGCodeFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            model.AddMillingPath(filePath);
        }

        ImGuiFileDialog::Instance()->Close();
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
    ImGui::SameLine();

    ImGui::BeginDisabled(!simulationRuns);
    if (ImGui::Button("Stop"))
        model.StopSimulation();

    ImGui::EndDisabled();
    ImGui::SameLine();

    ImGui::BeginDisabled(simulationRuns);
    if (ImGui::Button("Reset"))
        model.ResetSimulation();
    ImGui::EndDisabled();

    ImGui::SameLine();

    if (ImGui::Button("Instant"))
        model.StartInstantSimulation();
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

    ImGui::BeginDisabled(model.MillingMachineRuns());
    float height = cutter->height;
    if (ImGui::DragFloat("Height", &height))
        model.SetCutterHeight(height);
    ImGui::EndDisabled();
}


void MillingSimulatorView::RenderWarnings() const
{
    ImGui::SeparatorText("Warnings");

    auto const& warningsRepo = model.GetMillingWarnings();
    if (warningsRepo.Empty()) {
        ImGui::Text("No warnings");
    }

    for (const auto&[commandId, warningsTypes] : warningsRepo.GetWarnings()) {
        if (warningsTypes & MillingWarningsRepo::MillingStraightDown)
            ImGui::Text("Milling straight down during %d command", commandId);

        if (warningsTypes & MillingWarningsRepo::MillingTooDeep)
            ImGui::Text("Milling too deep during %d command", commandId);

        if (warningsTypes & MillingWarningsRepo::MillingUnderTheBase)
            ImGui::Text("Milling under the base during %d command", commandId);
    }
}




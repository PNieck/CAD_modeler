#include <CAD_modeler/views/guiView.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <list>
#include <ranges>


GuiView::GuiView(GLFWwindow * window)
{
    // FIXME: function, which generates glsl version string
    const char* glsl_version = "#version 330";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);
}


GuiView::~GuiView()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void GuiView::RenderGui(GuiController& controller, const Model& model) const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Modeler");
    
    switch (controller.GetAppState())
    {
        case AppState::Default:
            RenderDefaultGui(controller, model);
            break;

        case AppState::Adding3dPoints:
            RenderAdd3DPointsGui(controller, model);
            break;
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void GuiView::RenderDefaultGui(GuiController& controller, const Model& model) const
{
    if (ImGui::Button("Add 3D points")) {
        controller.SetAppState(AppState::Adding3dPoints);
    }

    if (ImGui::Button("Add torus")) {
        controller.AddTorus();
    }

    ImGui::Separator();

    RenderObjectNames(controller, model);
}


void GuiView::RenderObjectNames(GuiController & controller, const Model & model) const
{
    const auto& entities = model.EntitiesWithNames();
    static std::list<bool> selected;

    if (selected.size() != entities.size()) {
        selected.resize(entities.size(), false);
    }

    for (auto [entity, isSelected]: std::views::zip(entities, selected)) {
        ImGui::Selectable(
            model.GetEntityName(entity).c_str(),
            &isSelected
        );
    } 
}


void GuiView::RenderAdd3DPointsGui(GuiController & controller, const Model & model) const
{
    if (ImGui::Button("Finish adding points")) {
        controller.SetAppState(AppState::Default);
    }
}
#include <CAD_modeler/views/guiView.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <algorithm>
#include <iterator>


GuiView::GuiView(GLFWwindow* window, GuiController& controller, const Model& model):
    controller(controller), model(model)
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


void GuiView::RenderGui() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Modeler");
    
    switch (controller.GetAppState())
    {
        case AppState::Default:
            RenderDefaultGui();
            break;

        case AppState::Adding3dPoints:
            RenderAdd3DPointsGui();
            break;
    }

    ImGui::End();

    RenderObjectsProperties();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void GuiView::RenderDefaultGui() const
{
    if (ImGui::Button("Add 3D points")) {
        controller.SetAppState(AppState::Adding3dPoints);
    }

    if (ImGui::Button("Add torus")) {
        controller.AddTorus();
    }

    ImGui::Separator();

    RenderObjectsNames();
}


void GuiView::RenderObjectsNames() const
{
    const auto& entities = model.EntitiesWithNames();

    for (auto entity: entities) {
        bool oldSelected = model.IsSelected(entity);
        bool newSelected = oldSelected;

        ImGui::Selectable(
            model.GetEntityName(entity).c_str(),
            &newSelected
        );

        if (oldSelected != newSelected) {
            if (newSelected)
                controller.SelectEntity(entity);
            else
                controller.DeselectEntity(entity);
        }
    } 
}


void GuiView::RenderAdd3DPointsGui() const
{
    if (ImGui::Button("Finish adding points")) {
        controller.SetAppState(AppState::Default);
    }
}


void GuiView::RenderObjectsProperties() const
{
    auto const& selectedEntities = model.SelectedEntities();

    ImGui::Begin("Properties");

    switch (selectedEntities.size())
    {
    case 0:
        ImGui::Text("Select object to show it's properties");
        break;

    case 1:
        RenderSingleObjectProperties(*selectedEntities.begin());
        break;

    default:
        break;
    }

    ImGui::End();
}


void GuiView::RenderSingleObjectProperties(Entity entity) const
{
    auto const& components = model.GetEntityComponents(entity);

    auto it = components.find(Model::GetComponentId<Position>());
    if (it != components.end())
        DisplayPositionProperty(entity, model.GetPosition(entity));
}


void GuiView::DisplayPositionProperty(Entity entity, const Position& pos) const
{
    float x = pos.GetX();
    float y = pos.GetY();
    float z = pos.GetZ();

    ImGui::SeparatorText("Position");

    ImGui::DragFloat("X", &x, 0.01f);
    ImGui::DragFloat("Y", &y, 0.01f);
    ImGui::DragFloat("Z", &z, 0.01f);

    if (x != pos.GetX() || y != pos.GetY() || z != pos.GetZ())
        controller.ChangePosition(entity, Position(x, y, z));
}

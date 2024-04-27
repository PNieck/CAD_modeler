#include <CAD_modeler/views/guiView.hpp>

#include <CAD_modeler/utilities/angle.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <iterator>
#include <stdexcept>


GuiView::GuiView(GLFWwindow* window, GuiController& controller, const Model& model):
    controller(controller), model(model)
{
    // FIXME: function, which generates glsl version string
    const char* glsl_version = "#version 410";
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

        case AppState::AddingC0Curve:
            RenderAddingC0CurveGui();
            break;

        default:
            throw std::runtime_error("Unknown app state");
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

    if (ImGui::Button("Add C0 curve")) {
        controller.SetAppState(AppState::AddingC0Curve);
        controller.DeselectAllEntities();
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


void GuiView::RenderAddingC0CurveGui() const
{
    static std::unordered_set<Entity> controlPoints;
    static Entity curve;

    if (ImGui::Button("Finish adding points")) {
        controlPoints.clear();
        controller.SetAppState(AppState::Default);
        controller.DeselectAllEntities();
    }    

    ImGui::SeparatorText("Select control points");

    for (auto entity: model.GetAllPoints()) {
        bool oldSelected = false;

        if (controlPoints.contains(entity))
            oldSelected = true;

        bool newSelected = oldSelected;

        ImGui::Selectable(
            model.GetEntityName(entity).c_str(),
            &newSelected
        );

        if (oldSelected != newSelected) {
            if (newSelected) {
                controlPoints.insert(entity);
                if (controlPoints.size() == 1)
                    curve = controller.AddC0Curve({entity});
                else 
                    controller.AddC0CurveControlPoint(curve, entity);
                controller.SelectEntity(entity);
            }
            else {
                auto it = std::find(controlPoints.begin(), controlPoints.end(), entity);
                controlPoints.erase(it);
                if (controlPoints.size() == 0)
                    controller.DeleteEntity(curve);
                else
                    controller.DeleteC0ControlPoint(curve, entity);
                controller.DeselectEntity(entity);
            }
        }
    } 
}


void GuiView::RenderObjectsProperties() const
{
    auto const& selectedEntities = model.GetAllSelectedEntities();

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
        RenderMultipleObjectProperties();
        break;
    }

    ImGui::End();
}


void GuiView::RenderSingleObjectProperties(Entity entity) const
{
    auto const& components = model.GetEntityComponents(entity);

    auto it = components.find(Model::GetComponentId<Position>());
    if (it != components.end())
        DisplayPositionProperty(entity, model.GetComponent<Position>(entity));

    it = components.find(Model::GetComponentId<Scale>());
    if (it != components.end())
        DisplayScaleProperty(entity, model.GetComponent<Scale>(entity));

    it = components.find(Model::GetComponentId<Rotation>());
    if (it != components.end())
        DisplayRotationProperty(entity, model.GetComponent<Rotation>(entity));

    it = components.find(Model::GetComponentId<TorusParameters>());
    if (it != components.end())
        DisplayTorusProperty(entity, model.GetComponent<TorusParameters>(entity));

    it = components.find(Model::GetComponentId<Name>());
    if (it != components.end())
        DisplayNameEditor(entity, model.GetComponent<Name>(entity));

    it = components.find(Model::GetComponentId<C0CurveParameters>());
    if (it != components.end())
        DisplayBezierCurveProperty(entity, model.GetComponent<C0CurveParameters>(entity));

    if (ImGui::Button("Delete object")) {
        controller.DeleteEntity(entity);
    }
}


void GuiView::RenderMultipleObjectProperties() const
{
    Entity midPoint = model.GetMiddlePoint();
    const Position& pos = model.GetComponent<Position>(midPoint);

    float x = pos.GetX();
    float y = pos.GetY();
    float z = pos.GetZ();
    bool valueChanged = false;

    ImGui::SeparatorText("Position");

    valueChanged |= ImGui::DragFloat("X##Pos", &x, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Y##Pos", &y, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Z##Pos", &z, DRAG_FLOAT_SPEED);

    if (valueChanged)
        controller.TranslateSelected(Position(x, y, z));

    x = 1.0f;
    y = 1.0f;
    z = 1.0f;
    valueChanged = false;

    ImGui::SeparatorText("Scale");

    valueChanged |= ImGui::DragFloat("X##Scale", &x, DRAG_FLOAT_SPEED, MIN_SCALE, 0.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Y##Scale", &y, DRAG_FLOAT_SPEED, MIN_SCALE, 0.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Z##Scale", &z, DRAG_FLOAT_SPEED, MIN_SCALE, 0.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    if (valueChanged)
        controller.ScaleSelected(Scale(x, y, z));
    
    Rotation rotation;

    auto vector = rotation.GetRollPitchRoll();
    vector.X() = Angle::FromRadians(vector.X()).ToDegrees();
    vector.Y() = Angle::FromRadians(vector.Y()).ToDegrees();
    vector.Z() = Angle::FromRadians(vector.Z()).ToDegrees();

    valueChanged = false;

    ImGui::SeparatorText("Rotation");

    valueChanged |= ImGui::DragFloat("X##Rotation", &vector.X(), DRAG_ANGLE_SPEED, -180.0f, 180.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Y##Rotation", &vector.Y(), DRAG_ANGLE_SPEED, -90.0f, 90.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Z##Rotation", &vector.Z(), DRAG_ANGLE_SPEED, -180.0f, 180.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    if (valueChanged) {
        Rotation newRot(
            Angle::FromDegrees(vector.X()).ToRadians(),
            Angle::FromDegrees(vector.Y()).ToRadians(),
            Angle::FromDegrees(vector.Z()).ToRadians()
        );

        controller.RotateSelected(newRot);
    }
        
}


void GuiView::DisplayPositionProperty(Entity entity, const Position& pos) const
{
    float x = pos.GetX();
    float y = pos.GetY();
    float z = pos.GetZ();
    bool valueChanged = false;

    ImGui::SeparatorText("Position");

    valueChanged |= ImGui::DragFloat("X##Pos", &x, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Y##Pos", &y, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Z##Pos", &z, DRAG_FLOAT_SPEED);

    if (valueChanged)
        controller.ChangeComponent<Position>(entity, Position(x, y, z));  
}


void GuiView::DisplayScaleProperty(Entity entity, const Scale& scale) const
{
    float x = scale.GetX();
    float y = scale.GetY();
    float z = scale.GetZ();
    bool valueChanged = false;

    ImGui::SeparatorText("Scale");

    valueChanged |= ImGui::DragFloat("X##Scale", &x, DRAG_FLOAT_SPEED, MIN_SCALE, 0.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Y##Scale", &y, DRAG_FLOAT_SPEED, MIN_SCALE, 0.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Z##Scale", &z, DRAG_FLOAT_SPEED, MIN_SCALE, 0.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    if (valueChanged)
        controller.ChangeComponent<Scale>(entity, Scale(x, y, z));
}


void GuiView::DisplayRotationProperty(Entity entity, const Rotation & rotation) const
{
    auto vector = rotation.GetRollPitchRoll();
    vector.X() = Angle::FromRadians(vector.X()).ToDegrees();
    vector.Y() = Angle::FromRadians(vector.Y()).ToDegrees();
    vector.Z() = Angle::FromRadians(vector.Z()).ToDegrees();
    bool valueChanged = false;

    ImGui::SeparatorText("Rotation");

    valueChanged |= ImGui::DragFloat("X##Rotation", &vector.X(), DRAG_ANGLE_SPEED, -180.0f, 180.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Y##Rotation", &vector.Y(), DRAG_ANGLE_SPEED, -90.0f, 90.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Z##Rotation", &vector.Z(), DRAG_ANGLE_SPEED, -180.0f, 180.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    if (valueChanged) {
        Rotation newRot(
            Angle::FromDegrees(vector.X()).ToRadians(),
            Angle::FromDegrees(vector.Y()).ToRadians(),
            Angle::FromDegrees(vector.Z()).ToRadians()
        );

        controller.ChangeComponent<Rotation>(entity, newRot);
    }
}


void GuiView::DisplayTorusProperty(Entity entity, const TorusParameters& params) const
{
    float R = params.majorRadius;
    float r = params.minorRadius;
    int minDensity = params.meshDensityMinR;
    int majDensity = params.meshDensityMajR;

    bool valueChanged = false;

    ImGui::SeparatorText("Rotation");

    valueChanged |= ImGui::DragFloat("Major radius", &R, DRAG_FLOAT_SPEED, 0.001f, 0.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Minor radius", &r, DRAG_FLOAT_SPEED, 0.001f, R, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragInt("Mesh density along major radius", &majDensity, 0.2f, 3, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragInt("Mesh density along minor radius", &minDensity, 0.2f, 3, 100, "%d", ImGuiSliderFlags_AlwaysClamp);

    if (valueChanged) {
        TorusParameters newParams {
            .majorRadius = R,
            .minorRadius = r,
            .meshDensityMinR = minDensity,
            .meshDensityMajR = majDensity,
        };

        controller.ChangeComponent<TorusParameters>(entity, newParams);
    }
}


void GuiView::DisplayBezierCurveProperty(Entity entity, const C0CurveParameters& params) const
{
    bool drawPolygon = params.drawPolygon;

    ImGui::SeparatorText("Curve options");

    ImGui::Checkbox("Draw polygon", &drawPolygon);

    if (drawPolygon != params.drawPolygon) {

        // TODO: change copying whole params
        C0CurveParameters newParams = params;
        newParams.drawPolygon = drawPolygon;
        controller.ChangeComponent<C0CurveParameters>(entity, newParams);
    }

    ImGui::SeparatorText("Control Points");

    int selected = -1;
    int n = 0;
    for (auto controlPoint: params.ControlPoints()) {
        if (ImGui::Selectable(model.GetEntityName(controlPoint).c_str(), selected == n))
            selected = n;
        if (ImGui::BeginPopupContextItem()) {
            selected = n;
            if (ImGui::Button("Delete control point")) {
                controller.DeleteC0ControlPoint(entity, controlPoint);
            }
            ImGui::EndPopup();
        }

        ++n;
    }

    if (ImGui::Button("Add control points"))
        ImGui::OpenPopup("Add_c0_control_points_popup");
    
    if (ImGui::BeginPopup("Add_c0_control_points_popup")) {
        auto const& points = model.GetAllPoints();

        for (auto point: points) {
            if (std::find(params.ControlPoints().begin(), params.ControlPoints().end(), point) == params.ControlPoints().end()) {
                if (ImGui::Selectable(model.GetEntityName(point).c_str(), false)) {
                    controller.AddC0CurveControlPoint(entity, point);
                }
            }
        }

        ImGui::EndPopup();
    }
}


void GuiView::DisplayNameEditor(Entity entity, const Name& name) const
{
    Name tmp = name;

    ImGui::SeparatorText("Object name");

    if (ImGui::InputText("##objectName", &tmp))
        controller.ChangeEntityName(entity, tmp);
}

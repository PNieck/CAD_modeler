#include <CAD_modeler/views/guiView.hpp>

#include <CAD_modeler/utilities/angle.hpp>
#include <CAD_modeler/model/components/unremovable.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <optional>


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
            RenderAddingCurveGui(CurveType::C0);
            break;

        case AppState::AddingC2Curve:
            RenderAddingCurveGui(CurveType::C2);
            break;

        case AppState::AddingInterpolationCurve:
            RenderAddingCurveGui(CurveType::Interpolation);
            break;

        case AppState::AddingC0Surface:
            RenderAddingC0Surface();
            break;

        case AppState::AddingC0Cylinder:
            RenderAddingC0Cylinder();
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

    if (ImGui::Button("Add C2 curve")) {
        controller.SetAppState(AppState::AddingC2Curve);
        controller.DeselectAllEntities();
    }

    if (ImGui::Button("Add interpolation curve")) {
        controller.SetAppState(AppState::AddingInterpolationCurve);
        controller.DeselectAllEntities();
    }

    if (ImGui::Button("Add C0 surface")) {
        controller.SetAppState(AppState::AddingC0Surface);
    }

    if (ImGui::Button("Add C0 cylinder")) {
        controller.SetAppState(AppState::AddingC0Cylinder);
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


void GuiView::RenderAddingCurveGui(CurveType curveType) const
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
                    curve = controller.AddCurve({entity}, curveType);
                else 
                    controller.AddControlPointToCurve(curve, entity);
                controller.SelectEntity(entity);
            }
            else {
                auto it = std::find(controlPoints.begin(), controlPoints.end(), entity);
                controlPoints.erase(it);
                if (controlPoints.size() == 0)
                    controller.DeleteEntity(curve);
                else
                    controller.DeleteControlPointFromCurve(curve, entity);
                controller.DeselectEntity(entity);
            }
        }
    } 
}


void GuiView::RenderAddingC0Surface() const
{
    static std::optional<Entity> entity;
    static const alg::Vec3 dir(0.f, 1.f, 0.f);
    static float width, length;
    
    if (!entity.has_value()) {
        width = 1.0f;
        length = 1.0f;

        entity = controller.AddC0Surface(dir, length, width);
    }

    int rows = model.GetRowsCntOfC0Patches(entity.value());
    int cols = model.GetColsOfC0Patches(entity.value());
    bool valueChanged = false;

    ImGui::InputInt("Rows", &rows);
    ImGui::InputInt("Cols", &cols);

    valueChanged |= ImGui::DragFloat("Length", &length, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Width", &width, DRAG_FLOAT_SPEED);

    if (valueChanged)
        controller.RecalculateC0Surface(entity.value(), dir, length, width);

    if (rows != model.GetRowsCntOfC0Patches(entity.value())) {
        while (rows > model.GetRowsCntOfC0Patches(entity.value())) {
            controller.AddRowOfC0SurfacePatches(entity.value(), dir, length, width);
        }

        while (rows < model.GetRowsCntOfC0Patches(entity.value())) {
            controller.DeleteRowOfC0SurfacePatches(entity.value(), dir, length, width);
        }
    }

    if (cols != model.GetColsOfC0Patches(entity.value())) {
        while (cols > model.GetColsOfC0Patches(entity.value())) {
            controller.AddColOfC0SurfacePatches(entity.value(), dir, length, width);
        }

        while (cols < model.GetColsOfC0Patches(entity.value())) {
            controller.DeleteColOfC0SurfacePatches(entity.value(), dir, length, width);
        }
    }

    if (ImGui::Button("Finish adding")) {
        controller.SetAppState(AppState::Default);
        entity.reset();
    }
}


void GuiView::RenderAddingC0Cylinder() const
{
    static std::optional<Entity> entity;
    static float oldRadius, newRadius;
    static float oldLen, newLen;
    static const alg::Vec3 dir(0.f, 1.f, 0.f);
    
    if (!entity.has_value()) {
        entity = controller.AddC0Cylinder();
        oldRadius = newRadius = 1.0f;
        newLen = 1.f;
    }

    int rows = model.GetRowsCntOfC0Patches(entity.value());
    int cols = model.GetColsOfC0Patches(entity.value());
    bool valueChanged = false;

    ImGui::InputInt("Rows", &rows);
    ImGui::InputInt("Cols", &cols);

    valueChanged |= ImGui::DragFloat("Radius", &newRadius, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Length", &newLen, DRAG_FLOAT_SPEED);

    if (valueChanged) {
        controller.RecalculateC0Cylinder(entity.value(), dir * newLen, newRadius);
    }

    if (rows != model.GetRowsCntOfC0Patches(entity.value())) {
        while (rows > model.GetRowsCntOfC0Patches(entity.value())) {
            controller.AddRowOfC0CylinderPatches(entity.value(), dir * newLen, newRadius);
        }

        while (rows < model.GetRowsCntOfC0Patches(entity.value())) {
            controller.DeleteRowOfC0CylinderPatches(entity.value(), dir * newLen, newRadius);
        }
    }

    if (cols != model.GetColsOfC0Patches(entity.value())) {
        while (cols > model.GetColsOfC0Patches(entity.value())) {
            controller.AddColOfC0CylinderPatches(entity.value(), dir * newLen, newRadius);
        }

        while (cols < model.GetColsOfC0Patches(entity.value())) {
            controller.DeleteColOfC0CylinderPatches(entity.value(), dir * newLen, newRadius);
        }
    }

    if (ImGui::Button("Finish adding")) {
        controller.SetAppState(AppState::Default);
        entity.reset();
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

    it = components.find(Model::GetComponentId<ControlPoints>());
    if (it != components.end())
        DisplayCurveControlPoints(entity, model.GetComponent<ControlPoints>(entity));

    it = components.find(Model::GetComponentId<C0CurveParameters>());
    if (it != components.end())
        DisplayC0CurveParameters(entity, model.GetComponent<C0CurveParameters>(entity));

    it = components.find(Model::GetComponentId<C2CurveParameters>());
    if (it != components.end())
        DisplayC2CurveParameters(entity, model.GetComponent<C2CurveParameters>(entity));

    it = components.find(Model::GetComponentId<C0SurfaceDensity>());
    if (it != components.end())
        DisplaySurfaceDensityParameter(entity, model.GetComponent<C0SurfaceDensity>(entity));

    it = components.find(Model::GetComponentId<HasPatchesPolygon>());
    if (it != components.end())
        DisplayPatchesPolygonOption(entity, model.GetComponent<HasPatchesPolygon>(entity));

    it = components.find(Model::GetComponentId<Unremovable>());
    if (it == components.end())
        DisplayEntityDeletionOption(entity);
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


void GuiView::DisplayCurveControlPoints(Entity entity, const ControlPoints& params) const
{
    ImGui::SeparatorText("Control Points");

    int selected = -1;
    int n = 0;
    for (auto controlPoint: params.GetPoints()) {
        if (ImGui::Selectable(model.GetEntityName(controlPoint).c_str(), selected == n))
            selected = n;
        if (ImGui::BeginPopupContextItem()) {
            selected = n;
            if (ImGui::Button("Delete control point")) {
                controller.DeleteControlPointFromCurve(entity, controlPoint);
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
            // TODO: rewrite with set intersection
            if (std::find(params.GetPoints().begin(), params.GetPoints().end(), point) == params.GetPoints().end()) {
                if (ImGui::Selectable(model.GetEntityName(point).c_str(), false)) {
                    controller.AddControlPointToCurve(entity, point);
                }
            }
        }

        ImGui::EndPopup();
    }
}


void GuiView::DisplayC0CurveParameters(Entity entity, const C0CurveParameters & params) const
{
    bool drawPolygon = params.drawPolygon;

    ImGui::Checkbox("Draw polygon", &drawPolygon);

    if (drawPolygon != params.drawPolygon) {
        C0CurveParameters newParams;
        newParams.drawPolygon = drawPolygon;
        controller.ChangeComponent<C0CurveParameters>(entity, newParams);
    }
}


void GuiView::DisplayC2CurveParameters(Entity entity, const C2CurveParameters & params) const
{
    bool drawPolygon = params.drawBSplinePolygon;

    ImGui::Checkbox("Draw BSpline polygon", &drawPolygon);

    if (drawPolygon != params.drawBSplinePolygon) {
        if (drawPolygon)
            model.ShowC2BSplinePolygon(entity);
        else
            model.HideC2BSplinePolygon(entity);
    }

    drawPolygon = params.drawBezierPolygon;

    ImGui::Checkbox("Draw Bezier polygon", &drawPolygon);

    if (drawPolygon != params.drawBezierPolygon) {
        if (drawPolygon)
            model.ShowC2BezierPolygon(entity);
        else
            model.HideC2BezierPolygon(entity);
    }

    drawPolygon = params.showBezierControlPoints;

    ImGui::Checkbox("Show Bezier control points", &drawPolygon);

    if (drawPolygon != params.showBezierControlPoints) {
        if (drawPolygon)
            model.ShowC2BezierControlPoints(entity);
        else
            model.HideC2BezierControlPoints(entity);
    }
}


void GuiView::DisplayEntityDeletionOption(Entity entity) const
{
    if (ImGui::Button("Delete object")) {
        controller.DeleteEntity(entity);
    }
}


void GuiView::DisplaySurfaceDensityParameter(Entity entity, const C0SurfaceDensity &density) const
{
    int d = density.GetDensity();

    ImGui::DragInt("Mesh density", &d, 1.f, C0SurfaceDensity::MinDensity, C0SurfaceDensity::MaxDensity);

    if (d != density.GetDensity()) {
        controller.SetNewSurfaceDensity(entity, C0SurfaceDensity(d));
    }
}


void GuiView::DisplayPatchesPolygonOption(Entity entity, const HasPatchesPolygon &patchesPolygon) const
{
    bool value = patchesPolygon.value;

    ImGui::Checkbox("Draw Bezier Polygon", &value);

    if (value != patchesPolygon.value) {
        if (value)
            controller.ShowPatchesPolygon(entity);
        else
            controller.HidePatchesPolygon(entity);
    }
}


void GuiView::DisplayNameEditor(Entity entity, const Name& name) const
{
    Name tmp = name;

    ImGui::SeparatorText("Object name");

    if (ImGui::InputText("##objectName", &tmp))
        controller.ChangeEntityName(entity, tmp);
}

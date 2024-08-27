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
#include <sstream>


GuiView::GuiView(GLFWwindow* window, GuiController& controller, const Model& model):
    controller(controller), model(model), menuBar(controller, model)
{
    // FIXME: function, which generates glsl version string
    const char* glsl_version = "#version 410";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
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

    menuBar.Render();
    
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
            RenderAddingSurface(SurfaceType::C0);
            break;

        case AppState::AddingC2Surface:
            RenderAddingSurface(SurfaceType::C2);
            break;

        case AppState::AddingC0Cylinder:
            RenderAddingCylinder(CylinderType::C0);
            break;

        case AppState::AddingC2Cylinder:
            RenderAddingCylinder(CylinderType::C2);
            break;

        case AppState::AddingGregoryPatches:
            RenderAddingGregoryPatches();
            break;

        case AppState::AnaglyphsSettings:
            RenderAnaglyphsCameraSettings();
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

    if (ImGui::Button("Add C2 surface")) {
        controller.SetAppState(AppState::AddingC2Surface);
    }

    if (ImGui::Button("Add C0 cylinder")) {
        controller.SetAppState(AppState::AddingC0Cylinder);
    }

    if (ImGui::Button("Add C2 cylinder")) {
        controller.SetAppState(AppState::AddingC2Cylinder);
    }

    if (ImGui::Button("Add Gregory patches")) {
        model.DeselectAllEntities();
        controller.SetAppState(AppState::AddingGregoryPatches);
    }

    ImGui::Separator();

    RenderObjectsNames();
}


void GuiView::RenderObjectsNames() const
{
    const auto& entities = model.EntitiesWithNames();

    for (auto entity: entities) {
        bool selected = model.IsSelected(entity);

        if (ImGui::Selectable(
            model.GetEntityName(entity).c_str(),
            &selected
        )) {
            if (selected)
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
                    controller.AddControlPointToCurve(curve, entity, curveType);
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


// TODO: delete this function
int GetSurfaceRowsCnt(Entity surface, SurfaceType surfaceType, const Model& model)
{
    switch (surfaceType)
    {
    case SurfaceType::C0:
        return model.GetRowsCntOfC0Patches(surface);
    
    case SurfaceType::C2:
        return model.GetRowsCntOfC2Patches(surface);
    
    default:
        throw std::runtime_error("Unknown surface type");
    }
}


// TODO: delete this function
int GetSurfaceColsCnt(Entity surface, SurfaceType surfaceType, const Model& model)
{
    switch (surfaceType)
    {
    case SurfaceType::C0:
        return model.GetColsOfC0Patches(surface);
    
    case SurfaceType::C2:
        return model.GetColsCntOfC2Patches(surface);
    
    default:
        throw std::runtime_error("Unknown surface type");
    }
}


void GuiView::RenderAddingSurface(SurfaceType surfaceType) const
{
    static std::optional<Entity> entity;
    static const alg::Vec3 dir(0.f, 1.f, 0.f);
    static float width, length;
    
    if (!entity.has_value()) {
        width = 1.0f;
        length = 1.0f;

        entity = controller.AddSurface(surfaceType, dir, length, width);
    }

    int rows = GetSurfaceRowsCnt(entity.value(), surfaceType, model);
    int cols = GetSurfaceColsCnt(entity.value(), surfaceType, model);
    bool valueChanged = false;

    ImGui::InputInt("Rows", &rows);
    ImGui::InputInt("Cols", &cols);

    valueChanged |= ImGui::DragFloat("Length", &length, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Width", &width, DRAG_FLOAT_SPEED);

    if (valueChanged)
        controller.RecalculateSurface(entity.value(), surfaceType,  dir, length, width);

    if (rows != GetSurfaceRowsCnt(entity.value(), surfaceType, model)) {
        while (rows > GetSurfaceRowsCnt(entity.value(), surfaceType, model)) {
            controller.AddRowOfSurfacePatches(entity.value(), surfaceType, dir, length, width);
        }

        while (rows < GetSurfaceRowsCnt(entity.value(), surfaceType, model)) {
            controller.DeleteRowOfSurfacePatches(entity.value(), surfaceType, dir, length, width);
        }
    }

    if (cols != GetSurfaceColsCnt(entity.value(), surfaceType, model)) {
        while (cols > GetSurfaceColsCnt(entity.value(), surfaceType, model)) {
            controller.AddColOfSurfacePatches(entity.value(), surfaceType, dir, length, width);
        }

        while (cols < GetSurfaceColsCnt(entity.value(), surfaceType, model)) {
            controller.DeleteColOfSurfacePatches(entity.value(), surfaceType, dir, length, width);
        }
    }

    if (ImGui::Button("Finish adding")) {
        controller.SetAppState(AppState::Default);
        entity.reset();
    }
}

// TODO: delete this function
int GetCylinderRowsCnt(Entity cylinder, CylinderType CylinderType, const Model& model)
{
    switch (CylinderType)
    {
    case CylinderType::C0:
        return model.GetRowsCntOfC0Patches(cylinder);
    
    case CylinderType::C2:
        return model.GetRowsCntOfC2Cylinder(cylinder);
    
    default:
        throw std::runtime_error("Unknown surface type");
    }
}


// TODO: delete this function
int GetCylinderColsCnt(Entity cylinder, CylinderType CylinderType, const Model& model)
{
    switch (CylinderType)
    {
    case CylinderType::C0:
        return model.GetColsOfC0Patches(cylinder);
    
    case CylinderType::C2:
        return model.GetColsCntOfC2Cylinder(cylinder);
    
    default:
        throw std::runtime_error("Unknown surface type");
    }
}


void GuiView::RenderAddingCylinder(CylinderType cylinderType) const
{
    static std::optional<Entity> entity;
    static float oldRadius, newRadius;
    static float oldLen, newLen;
    static const alg::Vec3 dir(0.f, 1.f, 0.f);
    
    if (!entity.has_value()) {
        entity = controller.AddCylinder(cylinderType);
        oldRadius = newRadius = 1.0f;
        newLen = 1.f;
    }

    int rows = GetCylinderRowsCnt(entity.value(), cylinderType, model);
    int cols = GetCylinderColsCnt(entity.value(), cylinderType, model);
    bool valueChanged = false;

    ImGui::InputInt("Rows", &rows);
    ImGui::InputInt("Cols", &cols);

    valueChanged |= ImGui::DragFloat("Radius", &newRadius, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Length", &newLen, DRAG_FLOAT_SPEED);

    if (valueChanged) {
        controller.RecalculateCylinder(entity.value(), cylinderType, dir * newLen, newRadius);
    }

    if (rows != GetCylinderRowsCnt(entity.value(), cylinderType, model)) {
        while (rows > GetCylinderRowsCnt(entity.value(), cylinderType, model)) {
            controller.AddRowOfCylinderPatches(entity.value(), cylinderType, dir * newLen, newRadius);
        }

        while (rows < GetCylinderRowsCnt(entity.value(), cylinderType, model)) {
            controller.DeleteRowOfCylinderPatches(entity.value(), cylinderType, dir * newLen, newRadius);
        }
    }

    if (cols != GetCylinderColsCnt(entity.value(), cylinderType, model)) {
        while (cols > GetCylinderColsCnt(entity.value(), cylinderType, model)) {
            controller.AddColOfCylinderPatches(entity.value(), cylinderType, dir * newLen, newRadius);
        }

        while (cols < GetCylinderColsCnt(entity.value(), cylinderType, model)) {
            controller.DeleteColOfCylinderPatches(entity.value(), cylinderType, dir * newLen, newRadius);
        }
    }

    if (ImGui::Button("Finish adding")) {
        controller.SetAppState(AppState::Default);
        entity.reset();
    }
}


void GuiView::RenderAddingGregoryPatches() const
{
    static bool entitiesSelected = false;
    static std::vector<std::vector<Entity>> holes;
    static int selected = -1;

    if (!entitiesSelected) {
        ImGui::Text("Select up three C0 surfaces or patches");

        const auto& c0Surfaces = model.GetAllC0Surfaces();

        for (auto surface: c0Surfaces) {
            bool selected = model.IsSelected(surface);

            if (ImGui::Selectable(
                model.GetEntityName(surface).c_str(),
                &selected
            )) {
                if (selected) {
                    // TODO: add error message
                    if (model.GetAllSelectedEntities().size() < 3)
                        controller.SelectEntity(surface);
                }
                else
                    controller.DeselectEntity(surface);
            }
        }

        if (ImGui::Button("Accept")) {
            entitiesSelected = true;
            holes = model.GetHolesPossibleToFill(model.GetAllSelectedEntities());
            selected = -1;
        }

        if (ImGui::Button("Cancel")) {
            controller.SetAppState(AppState::Default);
            controller.DeselectAllEntities();
            entitiesSelected = false;
        }
    }
    else {
        ImGui::Text("Select a hole to fill");
        int holeNb = 1;

        for (auto& hole: holes) {
            std::stringstream ss;

            ss << "Hole " << holeNb;

            if (ImGui::Selectable(ss.str().c_str(), selected == holeNb)) {
                selected = holeNb;
                model.DeselectAllEntities();
                for (auto entity: hole) {
                    controller.SelectEntity(entity);
                }
            }

            holeNb++;
        }

        if (ImGui::Button("Cancel")) {
            controller.SetAppState(AppState::Default);
            controller.DeselectAllEntities();
            entitiesSelected = false;
            selected = -1;
        }
    }
    
}


void GuiView::RenderAnaglyphsCameraSettings() const
{
    float eyeSeparation = model.cameraManager.GetEyeSeparation();
    float convergence = model.cameraManager.GetConvergence();

    ImGui::Text("Anaglyphs camera settings");

    if (ImGui::DragFloat("Eye separation", &eyeSeparation, DRAG_FLOAT_SPEED))
        controller.SetEyeSeparation(eyeSeparation);
    
    if (ImGui::DragFloat("Convergence", &convergence, DRAG_FLOAT_SPEED))
        controller.SetConvergence(convergence);

    if (ImGui::Button("Ok"))
        controller.SetAppState(AppState::Default);
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

    if (selectedEntities.size() == 2) {
        Entity e1 = *selectedEntities.begin();
        Entity e2 = *(++selectedEntities.begin());

        RenderMergingControlPointsOptionButton(e1, e2);
    }

    ImGui::End();
}


void GuiView::RenderSingleObjectProperties(Entity entity) const
{
    auto const& components = model.GetEntityComponents(entity);

    if (components.contains(Model::GetComponentId<Position>()))
        DisplayPositionProperty(entity, model.GetComponent<Position>(entity));

    if (components.contains(Model::GetComponentId<Scale>()))
        DisplayScaleProperty(entity, model.GetComponent<Scale>(entity));

    if (components.contains(Model::GetComponentId<Rotation>()))
        DisplayRotationProperty(entity, model.GetComponent<Rotation>(entity));

    if (components.contains(Model::GetComponentId<TorusParameters>()))
        DisplayTorusProperty(entity, model.GetComponent<TorusParameters>(entity));

    if (components.contains(Model::GetComponentId<Name>()))
        DisplayNameEditor(entity, model.GetComponent<Name>(entity));

    if (components.contains(Model::GetComponentId<CurveControlPoints>()))
        DisplayCurveControlPoints(entity, model.GetComponent<CurveControlPoints>(entity));

    if (components.contains(Model::GetComponentId<C0CurveParameters>()))
        DisplayC0CurveParameters(entity, model.GetComponent<C0CurveParameters>(entity));

    if (components.contains(Model::GetComponentId<C2CurveParameters>()))
        DisplayC2CurveParameters(entity, model.GetComponent<C2CurveParameters>(entity));

    if (components.contains(Model::GetComponentId<PatchesDensity>()))
        DisplaySurfaceDensityParameter(entity, model.GetComponent<PatchesDensity>(entity));

    if (components.contains(Model::GetComponentId<C0Patches>()))
        DisplaySurfacePatches(entity, model.GetComponent<C0Patches>(entity));

    if (components.contains(Model::GetComponentId<C2Patches>()))
        DisplaySurfacePatches(entity, model.GetComponent<C2Patches>(entity));

    if (components.contains(Model::GetComponentId<C2CylinderPatches>()))
        DisplaySurfacePatches(entity, model.GetComponent<C2CylinderPatches>(entity));

    if (!components.contains(Model::GetComponentId<Unremovable>()))
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


void GuiView::RenderMergingControlPointsOptionButton(Entity e1, Entity e2) const
{
    ImGui::Separator();

    if (model.IsAControlPoint(e1) && model.IsAControlPoint(e2)) {
        if (ImGui::Button("Merge control points"))
            controller.MergeControlPoints(e1, e2);
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

    ImGui::SeparatorText("Torus parameters");

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


void GuiView::DisplayCurveControlPoints(Entity entity, const CurveControlPoints& params) const
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
        auto const& pointsWithNames = model.EntitiesWithNames();

        for (auto point: points) {
            // TODO: rewrite with set intersection
            if (std::find(params.GetPoints().begin(), params.GetPoints().end(), point) == params.GetPoints().end()) {
                if (pointsWithNames.contains(point) && ImGui::Selectable(model.GetEntityName(point).c_str(), false)) {
                    CurveType curveType;
                    
                    if (model.GetAllC0Curves().contains(point))
                        curveType = CurveType::C0;
                    else if (model.GetAllC2Curves().contains(point))
                        curveType = CurveType::C2;
                    else if (model.GetAllInterpolationCurves().contains(point))
                        curveType = CurveType::Interpolation;
                    else
                        throw std::runtime_error("Unknown curve type");

                    controller.AddControlPointToCurve(entity, point, curveType);
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


void GuiView::DisplaySurfaceDensityParameter(Entity entity, const PatchesDensity &density) const
{
    int d = density.GetDensity();

    ImGui::DragInt("Mesh density", &d, 1.f, PatchesDensity::MinDensity, PatchesDensity::MaxDensity);

    if (d != density.GetDensity()) {
        controller.SetNewSurfaceDensity(entity, PatchesDensity(d));
    }
}


void GuiView::DisplaySurfacePatches(Entity entity, const Patches &patches) const
{
    ImGui::SeparatorText("Control Points");

    for (int row=0; row < patches.PointsInRow(); row++) {
        for (int col=0; col < patches.PointsInCol(); col++) {
            Entity entity = patches.GetPoint(row, col);

            ImGui::Text(model.GetEntityName(entity).c_str());
        }
    }

    bool hasNet = model.HasPatchesPolygon(entity);

    if (ImGui::Checkbox("Draw Control Points Net", &hasNet)) {
        if (hasNet)
            controller.ShowPatchesPolygon(entity, patches);
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

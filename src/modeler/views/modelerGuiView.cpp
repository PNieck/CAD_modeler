#include <CAD_modeler/views/modelerGuiView.hpp>

#include <CAD_modeler/utilities/angle.hpp>
#include <CAD_modeler/model/components/unremovable.hpp>
#include <CAD_modeler/controllers/modelerController.hpp>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <iterator>
#include <stdexcept>
#include <optional>
#include <sstream>


ModelerGuiView::ModelerGuiView(ModelerController& controller, Modeler& model):
    controller(controller), model(model), menuBar(controller, model)
{
}


void ModelerGuiView::RenderGui()
{
    ImGui::Begin("Modeler");

    menuBar.Render();
    
    switch (controller.GetModelerState())
    {
        case ModelerState::Default:
            RenderDefaultGui();
            break;

        case ModelerState::Adding3dPoints:
            RenderAdd3DPointsGui();
            break;

        case ModelerState::AddingC0Curve:
            RenderAddingCurveGui(CurveType::C0);
            break;

        case ModelerState::AddingC2Curve:
            RenderAddingCurveGui(CurveType::C2);
            break;

        case ModelerState::AddingInterpolationCurve:
            RenderAddingCurveGui(CurveType::Interpolation);
            break;

        case ModelerState::AddingIntersectionCurve:
            RenderAddingIntersectionCurve();
            break;

        case ModelerState::AddingC0Surface:
            RenderAddingSurface(SurfaceType::C0);
            break;

        case ModelerState::AddingC2Surface:
            RenderAddingSurface(SurfaceType::C2);
            break;

        case ModelerState::AddingC0Cylinder:
            RenderAddingCylinder(CylinderType::C0);
            break;

        case ModelerState::AddingC2Cylinder:
            RenderAddingCylinder(CylinderType::C2);
            break;

        case ModelerState::AddingGregoryPatches:
            RenderAddingGregoryPatches();
            break;

        case ModelerState::AnaglyphsSettings:
            RenderAnaglyphsCameraSettings();
            break;

        default:
            throw std::runtime_error("Unknown app state");
    }

    ImGui::End();

    RenderObjectsProperties();
}


void ModelerGuiView::RenderDefaultGui()
{
    if (ImGui::Button("Add 3D points")) {
        controller.SetModelerState(ModelerState::Adding3dPoints);
    }

    if (ImGui::Button("Add torus")) {
        model.AddTorus();
    }

    if (ImGui::Button("Add C0 curve")) {
        controller.SetModelerState(ModelerState::AddingC0Curve);
        model.DeselectAllEntities();
    }

    if (ImGui::Button("Add C2 curve")) {
        controller.SetModelerState(ModelerState::AddingC2Curve);
        model.DeselectAllEntities();
    }

    if (ImGui::Button("Add interpolation curve")) {
        controller.SetModelerState(ModelerState::AddingInterpolationCurve);
        model.DeselectAllEntities();
    }

    if (ImGui::Button("Add intersection curve")) {
        controller.SetModelerState(ModelerState::AddingIntersectionCurve);
        model.DeselectAllEntities();
    }

    if (ImGui::Button("Add C0 surface")) {
        controller.SetModelerState(ModelerState::AddingC0Surface);
    }

    if (ImGui::Button("Add C2 surface")) {
        controller.SetModelerState(ModelerState::AddingC2Surface);
    }

    if (ImGui::Button("Add C0 cylinder")) {
        controller.SetModelerState(ModelerState::AddingC0Cylinder);
    }

    if (ImGui::Button("Add C2 cylinder")) {
        controller.SetModelerState(ModelerState::AddingC2Cylinder);
    }

    if (ImGui::Button("Add Gregory patches")) {
        controller.SetModelerState(ModelerState::AddingGregoryPatches);
        model.DeselectAllEntities();
    }

    if (ImGui::Button("Deselect all"))
        model.DeselectAllEntities();

    ImGui::Separator();

    RenderObjectsNames();
}


void ModelerGuiView::RenderObjectsNames() const
{
    static bool hidePoints = false;

    ImGui::Checkbox("Hide Points", &hidePoints);

    const auto& entities = model.EntitiesWithNames();
    const auto& points = model.GetAllPoints();

    for (auto entity: entities) {
        if (hidePoints && points.contains(entity))
            continue;

        bool selected = model.IsSelected(entity);

        if (ImGui::Selectable(
            model.GetEntityName(entity).c_str(),
            &selected
        )) {
            if (selected)
                model.Select(entity);
            else
                model.Deselect(entity);
        }
    } 
}


void ModelerGuiView::RenderAdd3DPointsGui()
{
    if (ImGui::Button("Finish adding points")) {
        controller.SetModelerState(ModelerState::Default);
    }
}


void ModelerGuiView::RenderAddingCurveGui(const CurveType curveType)
{
    static std::unordered_set<Entity> controlPoints;
    static Entity curve;

    if (ImGui::Button("Finish adding points")) {
        controlPoints.clear();
        controller.SetModelerState(ModelerState::Default);
        model.DeselectAllEntities();
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
                    curve = AddCurve({entity}, curveType);
                else 
                    AddControlPointToCurve(curve, entity, curveType);
                model.Select(entity);
            }
            else {
                const auto it = std::ranges::find(controlPoints, entity);
                controlPoints.erase(it);
                if (controlPoints.empty())
                    model.DeleteEntity(curve);
                else
                    model.DeleteControlPointFromCurve(curve, entity);
                model.Deselect(entity);
            }
        }
    } 
}


void ModelerGuiView::RenderAddingSurface(const SurfaceType surfaceType)
{
    static std::optional<Entity> entity;
    static constexpr alg::Vec3 dir(0.f, 1.f, 0.f);
    static float width, length;
    
    if (!entity.has_value()) {
        width = 1.0f;
        length = 1.0f;

        entity = AddSurface(surfaceType, dir, length, width);
    }

    int rows = GetSurfaceRowsCnt(entity.value(), surfaceType);
    int cols = GetSurfaceColsCnt(entity.value(), surfaceType);
    bool valueChanged = false;

    ImGui::InputInt("Rows", &rows);
    ImGui::InputInt("Cols", &cols);

    valueChanged |= ImGui::DragFloat("Length", &length, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Width", &width, DRAG_FLOAT_SPEED);

    if (valueChanged)
        RecalculateSurface(entity.value(), surfaceType,  dir, length, width);

    if (rows != GetSurfaceRowsCnt(entity.value(), surfaceType)) {
        while (rows > GetSurfaceRowsCnt(entity.value(), surfaceType)) {
            AddRowOfSurfacePatches(entity.value(), surfaceType, dir, length, width);
        }

        while (rows < GetSurfaceRowsCnt(entity.value(), surfaceType)) {
            DeleteRowOfSurfacePatches(entity.value(), surfaceType, dir, length, width);
        }
    }

    if (cols != GetSurfaceColsCnt(entity.value(), surfaceType)) {
        while (cols > GetSurfaceColsCnt(entity.value(), surfaceType)) {
            AddColOfSurfacePatches(entity.value(), surfaceType, dir, length, width);
        }

        while (cols < GetSurfaceColsCnt(entity.value(), surfaceType)) {
            DeleteColOfSurfacePatches(entity.value(), surfaceType, dir, length, width);
        }
    }

    if (ImGui::Button("Finish adding")) {
        controller.SetModelerState(ModelerState::Default);
        entity.reset();
    }
}


void ModelerGuiView::RenderAddingCylinder(const CylinderType cylinderType)
{
    static std::optional<Entity> entity;
    static float newRadius;
    static float newLen;
    static constexpr alg::Vec3 dir(0.f, 1.f, 0.f);
    
    if (!entity.has_value()) {
        entity = AddCylinder(cylinderType);
        newLen = 1.f;
    }

    int rows = GetCylinderRowsCnt(entity.value(), cylinderType);
    int cols = GetCylinderColsCnt(entity.value(), cylinderType);
    bool valueChanged = false;

    ImGui::InputInt("Rows", &rows);
    ImGui::InputInt("Cols", &cols);

    valueChanged |= ImGui::DragFloat("Radius", &newRadius, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Length", &newLen, DRAG_FLOAT_SPEED);

    if (valueChanged) {
        RecalculateCylinder(entity.value(), cylinderType, dir * newLen, newRadius);
    }

    if (rows != GetCylinderRowsCnt(entity.value(), cylinderType)) {
        while (rows > GetCylinderRowsCnt(entity.value(), cylinderType)) {
            AddRowOfCylinderPatches(entity.value(), cylinderType, dir * newLen, newRadius);
        }

        while (rows < GetCylinderRowsCnt(entity.value(), cylinderType)) {
            DeleteRowOfCylinderPatches(entity.value(), cylinderType, dir * newLen, newRadius);
        }
    }

    if (cols != GetCylinderColsCnt(entity.value(), cylinderType)) {
        while (cols > GetCylinderColsCnt(entity.value(), cylinderType)) {
            AddColOfCylinderPatches(entity.value(), cylinderType, dir * newLen, newRadius);
        }

        while (cols < GetCylinderColsCnt(entity.value(), cylinderType)) {
            DeleteColOfCylinderPatches(entity.value(), cylinderType, dir * newLen, newRadius);
        }
    }

    if (ImGui::Button("Finish adding")) {
        controller.SetModelerState(ModelerState::Default);
        entity.reset();
    }
}


void ModelerGuiView::RenderAddingGregoryPatches()
{
    static bool entitiesSelected = false;
    static std::vector<GregoryPatchesSystem::Hole> holes;
    static int selectedItem = -1;

    if (!entitiesSelected) {
        ImGui::Text("Select up three C0 surfaces or patches");

        const auto& c0Surfaces = model.GetAllC0Surfaces();

        for (const auto surface: c0Surfaces) {
            bool selected = model.IsSelected(surface);

            if (ImGui::Selectable(
                model.GetEntityName(surface).c_str(),
                &selected
            )) {
                if (selected) {
                    // TODO: add error message
                    if (model.GetAllSelectedEntities().size() < 3)
                        model.Select(surface);
                }
                else
                    model.Deselect(surface);
            }
        }

        if (ImGui::Button("Accept")) {
            entitiesSelected = true;
            holes = model.GetHolesPossibleToFill(model.GetAllSelectedEntities());
            selectedItem = -1;
        }

        if (ImGui::Button("Cancel")) {
            controller.SetModelerState(ModelerState::Default);
            model.DeselectAllEntities();
            entitiesSelected = false;
        }
    }
    else {
        ImGui::Text("Select a hole to fill");
        int holeNb = 1;

        for (auto& hole: holes) {
            std::stringstream ss;

            ss << "Hole " << holeNb;

            if (ImGui::Selectable(ss.str().c_str(), selectedItem == holeNb)) {
                selectedItem = holeNb;
                model.DeselectAllEntities();
                for (int i=0; i < GregoryPatchesSystem::Hole::innerCpNb; i++) {
                    model.Select(hole.GetInnerControlPoint(i));
                }
            }

            holeNb++;
        }

        if (ImGui::Button("Accept")) {
            model.FillHole(holes[selectedItem - 1]);
            controller.SetModelerState(ModelerState::Default);

            model.DeselectAllEntities();
            entitiesSelected = false;
            selectedItem = -1;
        }

        if (ImGui::Button("Cancel")) {
            controller.SetModelerState(ModelerState::Default);
            model.DeselectAllEntities();
            entitiesSelected = false;
            selectedItem = -1;
        }
    }
    
}


void ModelerGuiView::RenderAddingIntersectionCurve()
{
    ImGui::Text("Select two objects to intersect");

    const auto& c0Surfaces = model.GetAllC0Surfaces();
    if (!c0Surfaces.empty()) {
        ImGui::SeparatorText("C0 Surfaces");
        RenderSelectableEntitiesList(c0Surfaces);
    }

    const auto& c2Surfaces = model.GetAllC2Surfaces();
    if (!c2Surfaces.empty()) {
        ImGui::SeparatorText("C2 Surfaces");
        RenderSelectableEntitiesList(c2Surfaces);
    }


    const auto& c2Cylinders = model.GetAllC2Cylinders();
    if (!c2Surfaces.empty()) {
        ImGui::SeparatorText("C2 Cylinders");
        RenderSelectableEntitiesList(c2Cylinders);
    }

    const auto& tori = model.GetAllTori();
    if (!tori.empty()) {
        ImGui::SeparatorText("Tori");
        RenderSelectableEntitiesList(tori);
    }

    static float step = 0.1f;

    ImGui::DragFloat("Step", &step, 0.001f, 1e-5f);

    if (ImGui::Button("Accept")) {
        auto const& entities = model.GetAllSelectedEntities();

        if (entities.size() != 2)
            ImGui::OpenPopup("Wrong number of elements to intersect");
        else {
            const Entity e1 = *entities.begin();
            const Entity e2 = *(++entities.begin());
            model.FindIntersection(e1, e2, step);
        }
    }

    if (ImGui::BeginPopupModal("Wrong number of elements to intersect", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Wrong number of selected entities to intersect. Two objects must be selected");
        if (ImGui::Button("OK")) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }


    if (ImGui::Button("Cancel"))
        controller.SetModelerState(ModelerState::Default);
}


void ModelerGuiView::RenderAnaglyphsCameraSettings()
{
    float eyeSeparation = model.cameraManager.GetEyeSeparation();
    float convergence = model.cameraManager.GetConvergence();

    ImGui::Text("Anaglyphs camera settings");

    if (ImGui::DragFloat("Eye separation", &eyeSeparation, DRAG_FLOAT_SPEED))
        model.cameraManager.SetEyeSeparation(eyeSeparation);
    
    if (ImGui::DragFloat("Convergence", &convergence, DRAG_FLOAT_SPEED))
        model.cameraManager.SetConvergence(convergence);

    if (ImGui::Button("Ok"))
        controller.SetModelerState(ModelerState::Default);
}


void ModelerGuiView::RenderObjectsProperties() const
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
        const Entity e1 = *selectedEntities.begin();
        const Entity e2 = *(++selectedEntities.begin());

        RenderMergingControlPointsOptionButton(e1, e2);
    }

    ImGui::End();
}


void ModelerGuiView::RenderSingleObjectProperties(const Entity entity) const
{
    auto const& components = model.GetEntityComponents(entity);

    if (components.contains(Modeler::GetComponentId<Position>()))
        DisplayPositionProperty(entity, model.GetComponent<Position>(entity));

    if (components.contains(Modeler::GetComponentId<Scale>()))
        DisplayScaleProperty(entity, model.GetComponent<Scale>(entity));

    if (components.contains(Modeler::GetComponentId<Rotation>()))
        DisplayRotationProperty(entity, model.GetComponent<Rotation>(entity));

    if (components.contains(Modeler::GetComponentId<TorusParameters>()))
        DisplayTorusProperty(entity, model.GetComponent<TorusParameters>(entity));

    if (components.contains(Modeler::GetComponentId<Name>()))
        DisplayNameEditor(entity, model.GetComponent<Name>(entity));

    if (components.contains(Modeler::GetComponentId<CurveControlPoints>()))
        DisplayCurveControlPoints(entity, model.GetComponent<CurveControlPoints>(entity));

    if (components.contains(Modeler::GetComponentId<C0CurveParameters>()))
        DisplayC0CurveParameters(entity, model.GetComponent<C0CurveParameters>(entity));

    if (components.contains(Modeler::GetComponentId<C2CurveParameters>()))
        DisplayC2CurveParameters(entity, model.GetComponent<C2CurveParameters>(entity));

    if (components.contains(Modeler::GetComponentId<PatchesDensity>()))
        DisplaySurfaceDensityParameter(entity, model.GetComponent<PatchesDensity>(entity));

    if (components.contains(Modeler::GetComponentId<C0Patches>()))
        DisplaySurfacePatches(entity, model.GetComponent<C0Patches>(entity));

    if (components.contains(Modeler::GetComponentId<C2Patches>()))
        DisplaySurfacePatches(entity, model.GetComponent<C2Patches>(entity));

    if (components.contains(Modeler::GetComponentId<C2CylinderPatches>()))
        DisplaySurfacePatches(entity, model.GetComponent<C2CylinderPatches>(entity));

    if (components.contains(Modeler::GetComponentId<TriangleOfGregoryPatches>()))
        DisplayGregoryPatchesParameters(entity, model.GetComponent<TriangleOfGregoryPatches>(entity));

    if (!components.contains(Modeler::GetComponentId<Unremovable>()))
        DisplayEntityDeletionOption(entity);
}


void ModelerGuiView::RenderMultipleObjectProperties() const
{
    const Entity midPoint = model.GetMiddlePoint();
    const auto& pos = model.GetComponent<Position>(midPoint);

    float x = pos.GetX();
    float y = pos.GetY();
    float z = pos.GetZ();
    bool valueChanged = false;

    ImGui::SeparatorText("Position");

    valueChanged |= ImGui::DragFloat("X##Pos", &x, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Y##Pos", &y, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Z##Pos", &z, DRAG_FLOAT_SPEED);

    if (valueChanged)
        model.ChangeSelectedEntitiesPosition(Position(x, y, z));

    x = 1.0f;
    y = 1.0f;
    z = 1.0f;
    valueChanged = false;

    ImGui::SeparatorText("Scale");

    valueChanged |= ImGui::DragFloat("X##Scale", &x, DRAG_FLOAT_SPEED, MIN_SCALE, 0.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Y##Scale", &y, DRAG_FLOAT_SPEED, MIN_SCALE, 0.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    valueChanged |= ImGui::DragFloat("Z##Scale", &z, DRAG_FLOAT_SPEED, MIN_SCALE, 0.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    if (valueChanged)
        model.ChangeSelectedEntitiesScale(Scale(x, y, z));

    const Rotation rotation;

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
        const Rotation newRot(
            Angle::FromDegrees(vector.X()).ToRadians(),
            Angle::FromDegrees(vector.Y()).ToRadians(),
            Angle::FromDegrees(vector.Z()).ToRadians()
        );

        model.RotateSelectedEntities(newRot);
    }
}


void ModelerGuiView::RenderMergingControlPointsOptionButton(Entity e1, Entity e2) const
{
    ImGui::Separator();

    if (model.IsAControlPoint(e1) && model.IsAControlPoint(e2)) {
        if (ImGui::Button("Merge control points"))
            model.MergeControlPoints(e1, e2);
    }
}


void ModelerGuiView::DisplayPositionProperty(const Entity entity, const Position& pos) const
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
        model.SetComponent<Position>(entity, Position(x, y, z));
}


void ModelerGuiView::DisplayScaleProperty(const Entity entity, const Scale& scale) const
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
        model.SetComponent<Scale>(entity, Scale(x, y, z));
}


void ModelerGuiView::DisplayRotationProperty(const Entity entity, const Rotation& rotation) const
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
        const Rotation newRot(
            Angle::FromDegrees(vector.X()).ToRadians(),
            Angle::FromDegrees(vector.Y()).ToRadians(),
            Angle::FromDegrees(vector.Z()).ToRadians()
        );

        model.SetComponent<Rotation>(entity, newRot);
    }
}


void ModelerGuiView::DisplayTorusProperty(const Entity entity, const TorusParameters& params) const
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
        const TorusParameters newParams {
            .majorRadius = R,
            .minorRadius = r,
            .meshDensityMinR = minDensity,
            .meshDensityMajR = majDensity,
        };

        model.SetComponent<TorusParameters>(entity, newParams);
    }
}


void ModelerGuiView::DisplayCurveControlPoints(const Entity entity, const CurveControlPoints& params) const
{
    ImGui::SeparatorText("Control Points");

    int selected = -1;
    int n = 0;
    for (const auto controlPoint: params.GetPoints()) {
        if (ImGui::Selectable(model.GetEntityName(controlPoint).c_str(), selected == n))
            selected = n;
        if (ImGui::BeginPopupContextItem()) {
            selected = n;
            if (ImGui::Button("Delete control point")) {
                model.DeleteControlPointFromCurve(entity, controlPoint);
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
            if (std::ranges::find(params.GetPoints(), point) == params.GetPoints().end()) {
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

                    AddControlPointToCurve(entity, point, curveType);
                }
            }
        }

        ImGui::EndPopup();
    }
}


void ModelerGuiView::DisplayC0CurveParameters(const Entity entity, const C0CurveParameters & params) const
{
    bool drawPolygon = params.drawPolygon;

    ImGui::Checkbox("Draw polygon", &drawPolygon);

    if (drawPolygon != params.drawPolygon) {
        C0CurveParameters newParams;
        newParams.drawPolygon = drawPolygon;
        model.SetComponent<C0CurveParameters>(entity, newParams);
    }
}


void ModelerGuiView::DisplayC2CurveParameters(const Entity entity, const C2CurveParameters & params) const
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


void ModelerGuiView::DisplayEntityDeletionOption(const Entity entity) const
{
    if (ImGui::Button("Delete object")) {
        model.DeleteEntity(entity);
    }
}


void ModelerGuiView::DisplaySurfaceDensityParameter(const Entity entity, const PatchesDensity &density) const
{
    int d = density.GetDensity();

    ImGui::DragInt("Mesh density", &d, 1.f, PatchesDensity::MinDensity, PatchesDensity::MaxDensity);

    if (d != density.GetDensity()) {
        model.SetSurfaceDensity(entity, PatchesDensity(d));
    }
}


void ModelerGuiView::DisplaySurfacePatches(const Entity entity, const Patches &patches) const
{
    ImGui::SeparatorText("Control Points");

    bool hasNet = model.HasPatchesPolygon(entity);

    if (ImGui::Checkbox("Draw Control Points Net", &hasNet)) {
        if (hasNet)
            model.ShowPatchesPolygon(entity, patches);
        else
            model.HidePatchesPolygon(entity);
    }

    if (ImGui::Button("Select all control points")) {
        for (int row=0; row < patches.PointsInRow(); row++) {
            for (int col=0; col < patches.PointsInCol(); col++) {
                model.Select(patches.GetPoint(row, col));
            }
        }
    }

    ImGui::SeparatorText("Surface size");
    ImGui::Text("Patches in one direction: %d", patches.PatchesInCol());
    ImGui::Text("Patches in second direction: %d", patches.PatchesInRow());

    ImGui::SeparatorText("Control points");
    for (int row=0; row < patches.PointsInRow(); row++) {
        for (int col=0; col < patches.PointsInCol(); col++) {
            const Entity cp = patches.GetPoint(row, col);
            ImGui::Text(model.GetEntityName(cp).c_str());
        }
    }
}


void ModelerGuiView::DisplayGregoryPatchesParameters(const Entity entity, const TriangleOfGregoryPatches &triangle) const
{
    bool hasNet = triangle.hasNet;

    if (ImGui::Checkbox("Draw Control Points Net", &hasNet)) {
        if (hasNet)
            model.ShowGregoryNet(entity);
        else
            model.HideGregoryNet(entity);
    }
}


void ModelerGuiView::DisplayNameEditor(const Entity entity, const Name& name) const
{
    Name tmp = name;

    ImGui::SeparatorText("Object name");

    if (ImGui::InputText("##objectName", &tmp))
        model.ChangeEntityName(entity, tmp);
}


void ModelerGuiView::RenderSelectableEntitiesList(const std::unordered_set<Entity> &entities) const
{
    bool selected;

    for (const auto entity: entities) {
        selected = model.IsSelected(entity);

        if (ImGui::Selectable(
            model.GetEntityName(entity).c_str(),
            &selected
        )) {
            model.Select(entity);
        }
    }
}

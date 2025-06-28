#include <CAD_modeler/views/modeler/modelerMainMenuView.hpp>

#include <CAD_modeler/controllers/modelerController.hpp>
#include <CAD_modeler/views/globalViewParams.hpp>
#include <CAD_modeler/views/modeler/modelerGuiUtils.hpp>

#include <imgui.h>

#include <sstream>


ModelerMainMenuView::ModelerMainMenuView(ModelerController &controller, Modeler &model):
    controller(controller), model(model)
{
}


void ModelerMainMenuView::Render()
{
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
}


void ModelerMainMenuView::RenderDefaultGui() const
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


void ModelerMainMenuView::RenderAdd3DPointsGui()
{
    if (ImGui::Button("Finish adding points")) {
        controller.SetModelerState(ModelerState::Default);
    }
}


void ModelerMainMenuView::RenderAddingCurveGui(const CurveType curveType)
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
        bool oldSelected = controlPoints.contains(entity);
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
                    AddControlPointToCurve(model, curve, entity, curveType);
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


void ModelerMainMenuView::RenderAddingSurface(const SurfaceType surfaceType)
{
    static std::optional<Entity> entity;
    static constexpr alg::Vec3 dir(0.f, 1.f, 0.f);
    static float width, length;
    
    if (!entity.has_value()) {
        width = 1.0f;
        length = 1.0f;

        entity = AddPlane(surfaceType, dir, length, width);
    }

    int rows = GetSurfaceRowsCnt(entity.value(), surfaceType);
    int cols = GetSurfaceColsCnt(entity.value(), surfaceType);
    bool valueChanged = false;

    ImGui::InputInt("Rows", &rows);
    ImGui::InputInt("Cols", &cols);

    valueChanged |= ImGui::DragFloat("Length", &length, DRAG_FLOAT_SPEED);
    valueChanged |= ImGui::DragFloat("Width", &width, DRAG_FLOAT_SPEED);

    if (valueChanged)
        RecalculatePlane(entity.value(), surfaceType,  dir, length, width);

    if (rows != GetSurfaceRowsCnt(entity.value(), surfaceType)) {
        while (rows > GetSurfaceRowsCnt(entity.value(), surfaceType)) {
            AddRowOfPlanePatches(entity.value(), surfaceType, dir, length, width);
        }

        while (rows < GetSurfaceRowsCnt(entity.value(), surfaceType)) {
            DeleteRowOfPlanePatches(entity.value(), surfaceType, dir, length, width);
        }
    }

    if (cols != GetSurfaceColsCnt(entity.value(), surfaceType)) {
        while (cols > GetSurfaceColsCnt(entity.value(), surfaceType)) {
            AddColOfPlanePatches(entity.value(), surfaceType, dir, length, width);
        }

        while (cols < GetSurfaceColsCnt(entity.value(), surfaceType)) {
            DeleteColOfPlanePatches(entity.value(), surfaceType, dir, length, width);
        }
    }

    if (ImGui::Button("Finish adding")) {
        controller.SetModelerState(ModelerState::Default);
        entity.reset();
    }
}


void ModelerMainMenuView::RenderAddingCylinder(const CylinderType cylinderType)
{
    static std::optional<Entity> entity;
    static float newRadius;
    static float newLen;
    static constexpr alg::Vec3 dir(0.f, 1.f, 0.f);
    
    if (!entity.has_value()) {
        entity = AddCylinder(cylinderType);
        newLen = 1.f;
        newRadius = 1.0f;
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


void ModelerMainMenuView::RenderAddingGregoryPatches()
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


void ModelerMainMenuView::RenderAddingIntersectionCurve()
{
    ImGui::Text("Select object(s) to intersect");

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

    const auto& tori = model.GetAllTori();
    if (!tori.empty()) {
        ImGui::SeparatorText("Tori");
        RenderSelectableEntitiesList(tori);
    }

    static float step = 0.1f;
    static bool useGuidance = false;
    static std::optional<Entity> curve;

    ImGui::DragFloat("Step", &step, 0.001f, 1e-5f);

    ImGui::Checkbox("Use 3D cursor as guidance", &useGuidance);
    if (useGuidance) {
        const auto& cursorPos = model.GetCursorPosition();

        float x = cursorPos.GetX();
        float y = cursorPos.GetY();
        float z = cursorPos.GetZ();
        bool valueChanged = false;

        ImGui::Text("%s", "Cursor position");

        valueChanged |= ImGui::DragFloat("X##CursorPos", &x, DRAG_FLOAT_SPEED);
        valueChanged |= ImGui::DragFloat("Y##CursorPos", &y, DRAG_FLOAT_SPEED);
        valueChanged |= ImGui::DragFloat("Z##CursorPos", &z, DRAG_FLOAT_SPEED);

        if (valueChanged)
            model.SetCursorPosition(x, y, z);
    }

    if (ImGui::Button("Try")) {
        auto const& entities = model.GetAllSelectedEntities();

        if (entities.size() == 1) {
            if (curve.has_value())
                model.DeleteEntity(curve.value());

            if (useGuidance)
                curve = model.FindSelfIntersection(*entities.begin(), step, model.GetCursorPosition());
            else
                curve = model.FindSelfIntersection(*entities.begin(), step);

            if (!curve.has_value())
                ImGui::OpenPopup("Cannot find intersection curve");
        }
        else if (entities.size() == 2) {
            if (curve.has_value())
                model.DeleteEntity(curve.value());

            const Entity e1 = *entities.begin();
            const Entity e2 = *(++entities.begin());

            if (useGuidance)
                curve = model.FindIntersection(e1, e2, step, model.GetCursorPosition());
            else
                curve = model.FindIntersection(e1, e2, step);

            if (!curve.has_value())
                ImGui::OpenPopup("Cannot find intersection curve");
                    }
        else {
            ImGui::OpenPopup("Wrong number of elements to intersect");
        }
    }

    if (ImGui::BeginPopupModal("Wrong number of elements to intersect", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Wrong number of selected entities to intersect. One or two objects must be selected");
        if (ImGui::Button("OK")) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Cannot find intersection curve", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Cannot find intersection curve. Try to use 3d cursor");
        if (ImGui::Button("OK")) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    if (ImGui::Button("Accept")) {
        curve.reset();
        controller.SetModelerState(ModelerState::Default);
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
        if (curve.has_value()) {
            model.DeleteEntity(curve.value());
            curve.reset();
        }

        controller.SetModelerState(ModelerState::Default);
    }

}


void ModelerMainMenuView::RenderObjectsNames() const
{
    static bool hidePoints = false;

    ImGui::Checkbox("Hide Points", &hidePoints);

    const auto& entities = model.EntitiesWithNames();
    const auto& points = model.GetAllPoints();

    unsigned int id = 0;
    for (auto entity: entities) {
        if (hidePoints && points.contains(entity))
            continue;

        bool selected = model.IsSelected(entity);

        ImGui::PushID(++id);

        if (ImGui::Selectable(
            model.GetEntityName(entity).c_str(),
            &selected
        )) {
            if (selected)
                model.Select(entity);
            else
                model.Deselect(entity);
        }

        ImGui::PopID();
    } 
}


void ModelerMainMenuView::RenderAnaglyphsCameraSettings()
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


void ModelerMainMenuView::RenderSelectableEntitiesList(const std::unordered_set<Entity> &entities) const
{
    bool selected;

    for (const auto entity: entities) {
        selected = model.IsSelected(entity);

        if (ImGui::Selectable(model.GetEntityName(entity).c_str(), &selected)) {
            if (selected)
                model.Select(entity);
            else
                model.Deselect(entity);
        }
    }
}

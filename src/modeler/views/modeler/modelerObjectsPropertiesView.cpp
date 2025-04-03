#include <CAD_modeler/views/modeler/modelerObjectsPropertiesView.hpp>

#include <CAD_modeler/model/components/unremovable.hpp>
#include <CAD_modeler/utilities/angle.hpp>
#include <CAD_modeler/views/objectTypes/curvesTypes.hpp>
#include <CAD_modeler/views/globalViewParams.hpp>
#include <CAD_modeler/views/modeler/modelerGuiUtils.hpp>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <ranges>



ModelerObjectsPropertiesView::ModelerObjectsPropertiesView(ModelerController &controller, Modeler &model):
    controller(controller), model(model)
{
}


void ModelerObjectsPropertiesView::Render()
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


void ModelerObjectsPropertiesView::RenderSingleObjectProperties(Entity entity) const
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


void ModelerObjectsPropertiesView::RenderMultipleObjectProperties() const
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


void ModelerObjectsPropertiesView::RenderMergingControlPointsOptionButton(Entity e1, Entity e2) const
{
    ImGui::Separator();

    if (model.IsAControlPoint(e1) && model.IsAControlPoint(e2)) {
        if (ImGui::Button("Merge control points"))
            model.MergeControlPoints(e1, e2);
    }
}


void ModelerObjectsPropertiesView::DisplayPositionProperty(Entity entity, const Position &pos) const
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


void ModelerObjectsPropertiesView::DisplayScaleProperty(Entity entity, const Scale &scale) const
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


void ModelerObjectsPropertiesView::DisplayRotationProperty(Entity entity, const Rotation &rotation) const
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


void ModelerObjectsPropertiesView::DisplayTorusProperty(Entity entity, const TorusParameters &params) const
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


void ModelerObjectsPropertiesView::DisplayCurveControlPoints(Entity entity, const CurveControlPoints &params) const
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
        ImGui::OpenPopup("Add_control_points_popup");
    
    if (ImGui::BeginPopup("Add_control_points_popup")) {
        auto const& points = model.GetAllPoints();
        auto const& pointsWithNames = model.EntitiesWithNames();

        for (auto point: points) {
            // TODO: rewrite with set intersection
            if (std::ranges::find(params.GetPoints(), point) == params.GetPoints().end()) {
                if (pointsWithNames.contains(point) && ImGui::Selectable(model.GetEntityName(point).c_str(), false)) {
                    CurveType curveType = GetCurveType(model, entity);
                    AddControlPointToCurve(model, entity, point, curveType);
                }
            }
        }

        ImGui::EndPopup();
    }
}


void ModelerObjectsPropertiesView::DisplayC0CurveParameters(Entity entity, const C0CurveParameters &params) const
{
    bool drawPolygon = params.drawPolygon;

    ImGui::Checkbox("Draw polygon", &drawPolygon);

    if (drawPolygon != params.drawPolygon) {
        C0CurveParameters newParams;
        newParams.drawPolygon = drawPolygon;
        model.SetComponent<C0CurveParameters>(entity, newParams);
    }
}


void ModelerObjectsPropertiesView::DisplayC2CurveParameters(Entity entity, const C2CurveParameters &params) const
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


void ModelerObjectsPropertiesView::DisplayEntityDeletionOption(Entity entity) const
{
    if (ImGui::Button("Delete object")) {
        model.DeleteEntity(entity);
    }
}


void ModelerObjectsPropertiesView::DisplaySurfaceDensityParameter(Entity entity, const PatchesDensity &density) const
{
    int d = density.GetDensity();

    ImGui::DragInt("Mesh density", &d, 1.f, PatchesDensity::MinDensity, PatchesDensity::MaxDensity);

    if (d != density.GetDensity()) {
        model.SetSurfaceDensity(entity, PatchesDensity(d));
    }
}


void ModelerObjectsPropertiesView::DisplaySurfacePatches(const Entity entity, const Patches &patches) const
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

    // TODO: delete
    if (model.GetAllC0Surfaces().contains(entity)) {
        if (ImGui::Button("Show derivatives U")) {
            model.ShowDerivativesU(entity);
        }

        if (ImGui::Button("Show derivatives V")) {
            model.ShowDerivativesV(entity);
        }

        if (ImGui::Button("Show normals")) {
            model.ShowNormals(entity);
        }
    }
}


void ModelerObjectsPropertiesView::DisplayGregoryPatchesParameters(Entity entity, const TriangleOfGregoryPatches &triangle) const
{
    bool hasNet = triangle.hasNet;

    if (ImGui::Checkbox("Draw Control Points Net", &hasNet)) {
        if (hasNet)
            model.ShowGregoryNet(entity);
        else
            model.HideGregoryNet(entity);
    }
}


void ModelerObjectsPropertiesView::DisplayNameEditor(Entity entity, const Name &name) const
{
    Name tmp = name;

    ImGui::SeparatorText("Object name");

    if (ImGui::InputText("##objectName", &tmp))
        model.ChangeEntityName(entity, tmp);
}

#include <CAD_modeler/model/components/c0SurfacePatches.hpp>


C0SurfacePatches::C0SurfacePatches(int rows, int cols):
    patchesRows(rows), patchesCols(cols), controlPoints(PointsInRow())
{
    for (auto& row: controlPoints) {
        row.resize(PointsInCol());
    }
}


void C0SurfacePatches::AddRow()
{
    patchesRows++;
    controlPoints.reserve(controlPoints.size() + 3);

    controlPoints.push_back(std::move(std::vector<Entity>(PointsInCol())));
    controlPoints.push_back(std::move(std::vector<Entity>(PointsInCol())));
    controlPoints.push_back(std::move(std::vector<Entity>(PointsInCol())));
}


void C0SurfacePatches::AddCol()
{
    patchesCols++;

    for (auto& vec: controlPoints) {
        vec.reserve(PointsInCol());

        vec.push_back(Entity());
        vec.push_back(Entity());
        vec.push_back(Entity());
    }
}


void C0SurfacePatches::DeleteRow()
{
    if (patchesRows == 0)
        return;

    patchesRows--;

    controlPoints.resize(PointsInRow());
}


void C0SurfacePatches::DeleteCol()
{
    if (patchesCols == 0)
        return;

    patchesCols--;

    for (auto& vec: controlPoints) {
        vec.resize(PointsInCol());
    }
}

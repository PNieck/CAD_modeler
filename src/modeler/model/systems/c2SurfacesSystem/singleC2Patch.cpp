#include <CAD_modeler/model/systems/c2SurfacesSystem/singleC2Patch.hpp>

#include "CAD_modeler/model/systems/c2SurfacesSystem.hpp"


SingleC2Patch::SingleC2Patch(Coordinator &coord, const C2Patches &patches, const float u, const float v):
    coordinator(coord),
    patches(patches),
    firstRow(static_cast<int>(std::floor(u))),
    firstCol(static_cast<int>(std::floor(v)))
{
    if (v == C2SurfaceSystem::MaxV(patches))
        --firstRow;

    if (u == C2SurfaceSystem::MaxU(patches))
        --firstCol;
}

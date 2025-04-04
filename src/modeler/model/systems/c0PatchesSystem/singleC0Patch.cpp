#include <CAD_modeler/model/systems/c0PatchesSystem/singleC0Patch.hpp>

#include "CAD_modeler/model/systems/c0PatchesSystem.hpp"


SingleC0Patch::SingleC0Patch(Coordinator& coord, const C0Patches &patches, const float u, const float v):
    coordinator(coord),
    patches(patches),
    firstRow(static_cast<int>(std::floor(v)) * 3),
    firstCol(static_cast<int>(std::floor(u)) * 3)
{
    if (v == C0PatchesSystem::MaxV(patches))
        --firstRow;

    if (u == C0PatchesSystem::MaxU(patches))
        --firstCol;
}

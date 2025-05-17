#include <CAD_modeler/model/systems/c2PatchesSystem/singleC2Patch.hpp>

#include "CAD_modeler/model/systems/c2PatchesSystem.hpp"


SingleC2Patch::SingleC2Patch(Coordinator &coord, const C2Patches &patches, const float u, const float v):
    coordinator(coord),
    patches(patches),
    firstRow(static_cast<int>(std::floor(u))),
    firstCol(static_cast<int>(std::floor(v)))
{
    if (u == C2PatchesSystem::MaxU(patches))
        --firstRow;

    if (v == C2PatchesSystem::MaxV(patches))
        --firstCol;
}

#include <CAD_modeler/model/systems/c0PatchesSystem/singleC0Patch.hpp>


SingleC0Patch::SingleC0Patch(Coordinator& coord, const C0Patches &patches, float u, float v):
    coordinator(coord),
    patches(patches),
    firstRow(static_cast<int>(std::floor(v)) * 3),
    firstCol(static_cast<int>(std::floor(u)) * 3)
{
    if (v == static_cast<float>(patches.PatchesInRow()))
        --firstRow;

    if (u == static_cast<float>(patches.PatchesInCol()))
        --firstCol;
}

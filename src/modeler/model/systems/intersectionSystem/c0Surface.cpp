#include <CAD_modeler/model/systems/intersectionSystem/c0Surface.hpp>


bool ShouldWrapU(const C0Patches& patches)
{
    for (int col=0; col<patches.PointsInCol(); ++col) {
        if (patches.GetPoint(0, col) != patches.GetPoint(patches.PointsInRow() - 1, col))
            return false;
    }

    return true;
}


bool ShouldWrapV(const C0Patches& patches)
{
    for (int row=0; row<patches.PointsInRow(); ++row) {
        if (patches.GetPoint(row, 0) != patches.GetPoint(row, patches.PointsInCol() - 1))
            return false;
    }

    return true;
}


interSys::C0Surface::C0Surface(const Coordinator &coord, const Entity entity):
    patchesSys(coord.GetSystem<C0PatchesSystem>()),
    patches(coord.GetComponent<C0Patches>(entity)),
    wrapU(ShouldWrapU(patches)),
    wrapV(ShouldWrapV(patches))
{
}


alg::Vec3 interSys::C0Surface::PointOnSurface(float u, float v)
{
    Normalize(u, v);

    return patchesSys->PointOnPatches(patches, u, v).vec;
}


alg::Vec3 interSys::C0Surface::PartialDerivativeU(float u, float v)
{
    Normalize(u, v);

    return patchesSys->PartialDerivativeU(patches, u, v);
}


alg::Vec3 interSys::C0Surface::PartialDerivativeV(float u, float v)
{
    Normalize(u, v);

    return patchesSys->PartialDerivativeV(patches, u, v);
}


void interSys::C0Surface::Normalize(float &u, float &v) {
    if (wrapU) {
        if (const float maxU = C0PatchesSystem::MaxU(patches); u > maxU || u < 0.0f)
            u -= std::floor(u / maxU) * maxU;
    }

    if (wrapV) {
        if (const float maxV = C0PatchesSystem::MaxV(patches); v > maxV || v < 0.f)
            v -= std::floor(v / maxV) * maxV;
    }
}


float interSys::C0Surface::MaxU()
{
    if (wrapU)
        return std::numeric_limits<float>::infinity();

    return C0PatchesSystem::MaxU(patches);
}


float interSys::C0Surface::MinU()
{
    if (wrapU)
        return -std::numeric_limits<float>::infinity();

    return 0.f;
}


float interSys::C0Surface::MaxV()
{
    if (wrapU)
        return std::numeric_limits<float>::infinity();

    return C0PatchesSystem::MaxV(patches);
}


float interSys::C0Surface::MinV()
    { return MinU(); }

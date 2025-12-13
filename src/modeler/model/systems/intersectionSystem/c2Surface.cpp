#include <CAD_modeler/model/systems/intersectionSystem/c2Surface.hpp>

#include "CAD_modeler/model/components/wraps.hpp"


namespace interSys {
    C2Surface::C2Surface(const Coordinator &coord, const Entity entity):
        patchesSys(coord.GetSystem<C2PatchesSystem>()),
        patches(coord.GetComponent<C2Patches>(entity)),
        wrapU(coord.HasComponent<WrapU>(entity)),
        wrapV(coord.HasComponent<WrapV>(entity))
    {
    }


    alg::Vec3 C2Surface::PointOnSurface(float u, float v)
    {
        Normalize(u, v);

        return patchesSys->PointOnSurface(patches, u, v).vec;
    }


    alg::Vec3 C2Surface::PartialDerivativeU(float u, float v)
    {
        Normalize(u, v);

        return patchesSys->PartialDerivativeU(patches, u, v);
    }


    alg::Vec3 C2Surface::PartialDerivativeV(float u, float v)
    {
        Normalize(u, v);

        return patchesSys->PartialDerivativeV(patches, u, v);
    }


    void C2Surface::Normalize(float &u, float &v) {
        if (wrapU) {
            if (const float maxU = C2PatchesSystem::MaxU(patches); u > maxU || u < 0.0f)
                u -= std::floor(u / maxU) * maxU;
        }

        if (wrapV) {
            if (const float maxV = C2PatchesSystem::MaxV(patches); v > maxV || v < 0.f)
                v -= std::floor(v / maxV) * maxV;
        }
    }


    float C2Surface::MaxU()
    {
        if (wrapU)
            return std::numeric_limits<float>::infinity();

        return C2PatchesSystem::MaxU(patches);
    }


    float C2Surface::MinU()
    {
        if (wrapU)
            return -std::numeric_limits<float>::infinity();

        return 0.f;
    }


    float C2Surface::MaxV()
    {
        if (wrapV)
            return std::numeric_limits<float>::infinity();

        return C2PatchesSystem::MaxV(patches);
    }


    float C2Surface::MinV()
    {
        if (wrapV)
            return -std::numeric_limits<float>::infinity();

        return 0.f;
    }
}




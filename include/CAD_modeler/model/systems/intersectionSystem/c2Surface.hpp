#pragma once

#include "surfaces.hpp"

#include <ecs/coordinator.hpp>

#include "../c2SurfacesSystem.hpp"



namespace interSys
{
    class C2Surface final : public Surface {
    public:
        C2Surface(const Coordinator& coord, const Entity entity):
            surfaceSys(coord.GetSystem<C2SurfaceSystem>()), patches(coord.GetComponent<C2Patches>(entity)) {}

        ~C2Surface() override = default;

        alg::Vec3 PointOnSurface(const float u, const float v) override
            { return surfaceSys->PointOnPatches(patches, u, v).vec; }

        alg::Vec3 PartialDerivativeU(const float u, const float v) override
            { return surfaceSys->PartialDerivativeU(patches, u, v); }

        alg::Vec3 PartialDerivativeV(const float u, const float v) override
            { return surfaceSys->PartialDerivativeV(patches, u, v); }

        float MaxU() override
            { return C2SurfaceSystem::MaxU(patches); }

        float MaxV() override
            { return  C2SurfaceSystem::MaxV(patches); }


    private:
        std::shared_ptr<C2SurfaceSystem> surfaceSys;
        const C2Patches& patches;
    };
}

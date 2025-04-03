#pragma once

#include "surfaces.hpp"

#include <ecs/coordinator.hpp>

#include "../c0PatchesSystem.hpp"


namespace interSys
{
    class C0Surface final : public Surface {
    public:
        C0Surface(const Coordinator& coord, const Entity entity):
            patchesSys(coord.GetSystem<C0PatchesSystem>()), patches(coord.GetComponent<C0Patches>(entity)) {}

        ~C0Surface() override = default;

        alg::Vec3 PointOnSurface(const float u, const float v) override
            { return patchesSys->PointOnPatches(patches, u, v).vec; }

        alg::Vec3 PartialDerivativeU(const float u, const float v) override
            { return patchesSys->PartialDerivativeU(patches, u, v); }

        alg::Vec3 PartialDerivativeV(const float u, const float v) override
            { return patchesSys->PartialDerivativeV(patches, u, v); }

        float MaxU() override
            { return C0PatchesSystem::MaxU(patches); }

        float MaxV() override
            { return  C0PatchesSystem::MaxV(patches); }


    private:
        std::shared_ptr<C0PatchesSystem> patchesSys;
        const C0Patches& patches;
    };
}

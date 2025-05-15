#pragma once

#include "surface.hpp"

#include "../c0PatchesSystem.hpp"


namespace interSys
{
    class C0Surface final : public Surface {
    public:
        C0Surface(const Coordinator& coord, Entity entity);

        ~C0Surface() override = default;

        alg::Vec3 PointOnSurface(float u, float v) override;

        alg::Vec3 PartialDerivativeU(float u, float v) override;

        alg::Vec3 PartialDerivativeV(float u, float v) override;

        void Normalize(float &u, float &v) override;

        float MaxUSampleVal() override
            { return C0PatchesSystem::MaxU(patches); }

        float MaxVSampleVal() override
            { return C0PatchesSystem::MaxV(patches); }

        float MaxU() override;
        float MinU() override;

        float MaxV() override;
        float MinV() override;
    private:
        std::shared_ptr<C0PatchesSystem> patchesSys;
        const C0Patches& patches;

        const bool wrapU;
        const bool wrapV;
    };
}

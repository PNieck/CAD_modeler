#pragma once

#include "surface.hpp"

#include "../c2PatchesSystem.hpp"


namespace interSys
{
    class C2Surface : public Surface {
    public:
        C2Surface(const Coordinator &coord, Entity entity);

        ~C2Surface() override = default;

        alg::Vec3 PointOnSurface(float u, float v) override;

        alg::Vec3 PartialDerivativeU(float u, float v) override;

        alg::Vec3 PartialDerivativeV(float u, float v) override;

        void Normalize(float &u, float &v) override;

        float MaxUSampleVal() override
            { return C2PatchesSystem::MaxU(patches); }

        float MaxVSampleVal() override
            { return C2PatchesSystem::MaxV(patches); }

        float MaxU() override;

        float MaxV() override;

        float MinU() override;

        float MinV() override;

    private:
        std::shared_ptr<C2PatchesSystem> patchesSys;
        const C2Patches& patches;

        const bool wrapU;
        const bool wrapV;
    };
}

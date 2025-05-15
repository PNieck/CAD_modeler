#include <CAD_modeler/model/systems/intersectionSystem/c2Surface.hpp>


namespace interSys {

    bool ShouldWrapU(const C2Patches& patches) {
        for (int col=0; col < patches.PointsInCol(); ++col) {
            for (int row=0; row < 3; ++row) {
                if (patches.GetPoint(row, col) != patches.GetPoint(patches.PointsInRow() + row - 3, col))
                    return false;
            }
        }

        return true;
    }


    bool ShouldWrapV(const C2Patches& patches) {
        for (int row=0; row < patches.PointsInRow(); ++row) {
            for (int col=0; col < 3; ++col) {
                if (patches.GetPoint(row, col) != patches.GetPoint(row, patches.PointsInCol() + col - 3))
                    return false;
            }
        }

        return true;
    }


    C2Surface::C2Surface(const Coordinator &coord, const C2Patches& patches):
        surfaceSys(coord.GetSystem<C2SurfaceSystem>()),
        patches(patches),
        wrapU(ShouldWrapU(patches)),
        wrapV(ShouldWrapV(patches))
    {
    }


    alg::Vec3 C2Surface::PointOnSurface(float u, float v)
    {
        Normalize(u, v);

        return surfaceSys->PointOnSurface(patches, u, v).vec;
    }


    alg::Vec3 C2Surface::PartialDerivativeU(float u, float v)
    {
        Normalize(u, v);

        return surfaceSys->PartialDerivativeU(patches, u, v);
    }


    alg::Vec3 C2Surface::PartialDerivativeV(float u, float v)
    {
        Normalize(u, v);

        return surfaceSys->PartialDerivativeV(patches, u, v);
    }


    void C2Surface::Normalize(float &u, float &v) {
        if (wrapU) {
            if (const float maxU = C2SurfaceSystem::MaxU(patches); u > maxU || u < 0.0f)
                u -= std::floor(u / maxU) * maxU;
        }

        if (wrapV) {
            if (const float maxV = C2SurfaceSystem::MaxV(patches); v > maxV || v < 0.f)
                v -= std::floor(v / maxV) * maxV;
        }
    }


    float C2Surface::MaxU()
    {
        if (wrapU)
            return std::numeric_limits<float>::infinity();

        return C2SurfaceSystem::MaxU(patches);
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

        return C2SurfaceSystem::MaxV(patches);
    }


    float C2Surface::MinV()
    {
        if (wrapV)
            return -std::numeric_limits<float>::infinity();

        return 0.f;
    }
}




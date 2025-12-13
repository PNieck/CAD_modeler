#include <CAD_modeler/model/systems/surfaceSystem.hpp>


alg::Vec3 SurfaceSystem::PartialDerivativeUUApprox(Entity e, float u, float v) const
{
    constexpr float eps = 1e-6f;

    if (u + eps > MaxU(e)) {
        const alg::Vec3 v1 = PartialDerivativeU(e, u, v);
        const alg::Vec3 v2 = PartialDerivativeU(e, u - eps, v);

        return (v1 - v2) / eps;
    }

    if (u - eps < 0.f) {
        const alg::Vec3 v1 = PartialDerivativeU(e, u + eps, v);
        const alg::Vec3 v2 = PartialDerivativeU(e, u, v);

        return (v1 - v2) / eps;
    }

    const alg::Vec3 v1 = PartialDerivativeU(e, u + eps, v);
    const alg::Vec3 v2 = PartialDerivativeU(e, u - eps, v);

    return (v1 - v2) / (2.f * eps);
}


alg::Vec3 SurfaceSystem::PartialDerivativeVVApprox(Entity e, float u, float v) const
{
    constexpr float eps = 1e-6f;

    if (v + eps > MaxV(e)) {
        const alg::Vec3 v1 = PartialDerivativeV(e, u, v);
        const alg::Vec3 v2 = PartialDerivativeV(e, u, v - eps);

        return (v1 - v2) / eps;
    }

    if (v - eps < 0.f) {
        const alg::Vec3 v1 = PartialDerivativeV(e, u, v + eps);
        const alg::Vec3 v2 = PartialDerivativeV(e, u, v);

        return (v1 - v2) / eps;
    }

    const alg::Vec3 v1 = PartialDerivativeV(e, u, v + eps);
    const alg::Vec3 v2 = PartialDerivativeV(e, u, v - eps);

    return (v1 - v2) / (2.f * eps);
}


alg::Vec3 SurfaceSystem::PartialDerivativeUVApprox(Entity e, float u, float v) const
{
    constexpr float eps = 1e-6f;

    if (v + eps > MaxV(e)) {
        const alg::Vec3 v1 = PartialDerivativeU(e, u, v);
        const alg::Vec3 v2 = PartialDerivativeU(e, u, v - eps);

        return (v1 - v2) / eps;
    }

    if (v - eps < 0.f) {
        const alg::Vec3 v1 = PartialDerivativeU(e, u, v + eps);
        const alg::Vec3 v2 = PartialDerivativeU(e, u, v);

        return (v1 - v2) / eps;
    }

    const alg::Vec3 v1 = PartialDerivativeU(e, u, v + eps);
    const alg::Vec3 v2 = PartialDerivativeU(e, u, v - eps);

    return (v1 - v2) / (2.f * eps);
}

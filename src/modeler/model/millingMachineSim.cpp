#include "CAD_modeler/model/millingMachineSim.hpp"


MillingMachineSim::MillingMachineSim(const int viewportWidth, const int viewportHeight):
    Model(viewportWidth, viewportHeight)
{

}


void MillingMachineSim::RenderSystemsObjects(const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, float nearPlane,
    float farPlane) const {
}

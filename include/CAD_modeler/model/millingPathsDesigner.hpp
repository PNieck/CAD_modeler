#pragma  once

#include "model.hpp"


class MillingPathsDesigner: public Model {
public:
    MillingPathsDesigner(int viewportWidth, int viewportHeight);

protected:
    void RenderSystemsObjects(
        const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, float nearPlane, float farPlane
    ) const override;

private:

};

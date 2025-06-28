#pragma once

#include <ecs/system.hpp>

#include "algebra/mat4x4.hpp"


class C2PatchesRenderSystem final : public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Render(const alg::Mat4x4& cameraMtx) const;
};

#pragma once

#include <ecs/system.hpp>
#include "../components/position.hpp"


class PolylineSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity AddPolyline(const std::vector<Position>& pos);

    void Render(const alg::Mat4x4& cameraMtx) const;

private:
    std::vector<float> GenerateBezierPolygonVertices(const std::vector<Position>& cps) const;
    std::vector<uint32_t> GenerateBezierPolygonIndices(const std::vector<Position>& cps) const;
};

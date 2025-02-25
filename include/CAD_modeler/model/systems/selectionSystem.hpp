#pragma once

#include <ecs/system.hpp>

#include "../components/mesh.hpp"
#include "../components/position.hpp"
#include "../components/rotation.hpp"
#include "../components/scale.hpp"
#include "../../utilities/line.hpp"

#include <vector>


class SelectionSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    SelectionSystem();

    void Init();

    inline bool IsSelected(Entity entity) const
        { return entities.contains(entity); }

    void Select(Entity entity);

    void Deselect(Entity entity);

    void DeselectAllEntities();

    // TODO: change to ray
    void SelectFromLine(const Line& line);

    void SelectAllFromLine(const Line& line, float maxDist);

    void RenderMiddlePoint(const alg::Mat4x4& cameraMtx);

    inline Entity GetMiddlePoint() const
        { return middlePoint; }

    void MoveSelected(const Position& newMiddlePointPos);

    void ScaleSelected(const Scale& scale);

    void RotateSelected(const Rotation& rotation);

    void RenderSelectionCircle(float x, float y);

private:
    Mesh pointsMesh;
    Mesh circleMesh;
    Entity middlePoint;

    void UpdateMiddlePointPosition();

    std::vector<float> GenerateSelectionCircleVertices(float x, float y, float radius);
    std::vector<uint32_t> GenerateSelectionCircleIndices();
};

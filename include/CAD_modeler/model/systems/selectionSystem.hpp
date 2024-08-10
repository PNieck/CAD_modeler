#pragma once

#include <ecs/system.hpp>
#include "../components/mesh.hpp"
#include "../components/position.hpp"
#include "../components/rotation.hpp"
#include "../components/scale.hpp"
#include "shaders/shaderRepository.hpp"
#include "../../utilities/line.hpp"


class SelectionSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    SelectionSystem();

    void Init(ShaderRepository* shaderRepo);

    inline bool IsSelected(Entity entity) const
        { return entities.contains(entity); }

    void Select(Entity entity);

    void Deselect(Entity entity);

    void DeselectAllEntities();

    // TODO: change to ray
    void SelectFromLine(const Line& line);

    void RenderMiddlePoint(const alg::Mat4x4& cameraMtx);

    inline Entity GetMiddlePoint() const
        { return middlePoint; }

    void MoveSelected(const Position& newMiddlePointPos);

    void ScaleSelected(const Scale& scale);

    void RotateSelected(const Rotation& rotation);

private:
    Mesh pointsMesh;
    ShaderRepository* shadersRepo;
    Entity middlePoint;

    void UpdateMiddlePointPosition();
};

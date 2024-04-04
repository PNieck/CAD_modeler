#pragma once

#include "system.hpp"
#include "../components/mesh.hpp"
#include "../../shader.hpp"
#include "../../utilities/line.hpp"


class SelectionSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    SelectionSystem();

    void Init();

    inline bool IsSelected(Entity entity) const
        { return entities.contains(entity); }

    void Select(Entity entity);

    void Deselect(Entity entity);

    inline const std::unordered_set<Entity>& SelectedEntities() const
        { return entities; }

    void SelectFromLine(const Line& line);

    void RenderMiddlePoint();

    inline Entity GetMiddlePoint() const
        { return middlePoint; }

private:
    Mesh pointsMesh;
    Shader shader;

    Entity middlePoint;

    void UpdateMiddlePointPosition();
};

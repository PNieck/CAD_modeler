#pragma once

#include <string>

#include <ecs/coordinator.hpp>


class SaveManager {
public:
    void SaveScene(const std::string& path, Coordinator& coordinator);
};
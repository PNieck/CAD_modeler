#pragma once

#include <string>

#include <ecs/coordinator.hpp>


class SaveManager {
public:
    void LoadScene(const std::string& path, Coordinator& coordinator);

    void SaveScene(const std::string& path, Coordinator& coordinator);
};
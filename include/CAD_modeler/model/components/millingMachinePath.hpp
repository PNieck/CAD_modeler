#pragma once

#include <utility>
#include <vector>

#include "position.hpp"


class MoveCommand {
public:
    MoveCommand(const int id, Position destination):
        id(id), destination(std::move(destination)) {}

    MoveCommand(const int id, const float destX, const float destY, const float destZ):
        id(id), destination(destX, destY, destZ) {}

    int id{};
    Position destination;
};


class MillingMachinePath {
public:
    [[nodiscard]]
    auto begin()
        { return commands.begin(); }

    [[nodiscard]]
    auto end()
        { return commands.end(); }

    [[nodiscard]]
    auto begin() const
        { return commands.begin(); }

    [[nodiscard]]
    auto end() const
        { return commands.end(); }

    [[nodiscard]]
    auto cbegin() const
        { return commands.cbegin(); }

    [[nodiscard]]
    auto cend() const
        { return commands.cend(); }

    std::vector<MoveCommand> commands;
};
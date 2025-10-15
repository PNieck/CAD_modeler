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
    std::vector<MoveCommand> commands;
};
#pragma once

#include "../components/millingMachinePath.hpp"


class MillingMachinePathsBuilder {
public:
    MillingMachinePathsBuilder() = default;

    void AddPosition(const Position& nextPosition)
        { path.commands.emplace_back(nextID++, nextPosition); }

    void AddPosition(const alg::Vec3& vec)
        { path.commands.emplace_back(nextID++, vec); }

    void AddPositionFromOffset(const alg::Vec3& offset) {
        const auto& lastCommand = path.commands.back();
        auto newPos = lastCommand.destination.vec + offset;

        path.commands.emplace_back(nextID++, newPos);
    }

    [[nodiscard]]
    Position GetLastPosition() const
        { return path.commands.back().destination; }

    MillingMachinePath GetPaths()
        { return { std::move(path) }; }

private:
    int nextID = 1;
    MillingMachinePath path;
};

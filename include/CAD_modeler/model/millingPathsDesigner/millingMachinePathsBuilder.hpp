#pragma once

#include "../components/millingMachinePath.hpp"


class MillingMachinePathsBuilder {
public:
    explicit MillingMachinePathsBuilder(const float eps = 1e-6):
        eps(eps) {}

    void AddPosition(const Position& nextPosition);

    void AddPosition(const alg::Vec3& vec)
        { AddPosition(Position(vec)); }

    void AddPosition(const float x, const float y, const float z)
        { AddPosition(Position(x, y, z)); }

    void AddPositionFromOffset(const alg::Vec3& offset);

    [[nodiscard]]
    const Position& GetLastPosition() const
        { return path.commands.back().destination; }

    void PopLastPosition()
        { path.commands.pop_back(); }

    MillingMachinePath GetPaths()
        { return { std::move(path) }; }

    [[nodiscard]]
    size_t GetCommandsCnt() const
        { return path.commands.size(); }

private:
    float eps;

    int nextID = 1;
    MillingMachinePath path;
};

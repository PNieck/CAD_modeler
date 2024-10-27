#pragma once

#include <forward_list>

#include "position.hpp"


class MillingMachinePaths {
public:
    void PushFront(const Position& pos)
    {
        positions.push_front(pos);
        size++;
    }

    [[nodiscard]]
    const std::forward_list<Position>& GetPositions() const
        { return positions; }

    [[nodiscard]]
    size_t GetSize() const
        { return size; }

private:
    std::forward_list<Position> positions;
    size_t size = 0;
};
#pragma once

#include <unordered_set>


template <typename T>
std::unordered_set<T> intersect(const std::unordered_set<T>& set1, const std::unordered_set<T>& set2)
{
    // Set1 is not bigger than set2
    if (set1.size() > set2.size())
        return intersect(set2, set1);

    std::unordered_set<T> result;

    for (auto const& elem: set1) {
        if (set2.contains(elem)) {
            result.insert(elem);
        }
    }

    return result;
}

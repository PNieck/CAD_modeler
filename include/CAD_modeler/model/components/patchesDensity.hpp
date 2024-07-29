#pragma once

#include <cassert>


class PatchesDensity {
public:
    static constexpr int MinDensity = 2;
    static constexpr int MaxDensity = 64;

    PatchesDensity(int density):
        density(density) {
            CheckDensity();
        }

    inline int GetDensity() const
        { return density; }

    void SetDensity(int newDensity) {
        density = newDensity;
        CheckDensity();
    }

private:
    int density;

    void CheckDensity() const {
        assert(density >= MinDensity);
        assert(density <= MaxDensity);
    }
};

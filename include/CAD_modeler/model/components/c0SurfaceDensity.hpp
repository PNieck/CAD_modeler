#pragma once

#include <cassert>


class C0SurfaceDensity {
public:
    static constexpr int MinDensity = 2;
    static constexpr int MaxDensity = 64;

    C0SurfaceDensity(int density):
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

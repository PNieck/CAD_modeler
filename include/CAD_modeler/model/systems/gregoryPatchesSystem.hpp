#pragma once

#include "../components/c0Patches.hpp"

#include <ecs/system.hpp>

#include <vector>


class GregoryPatchesSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    class Hole {
    public:
        static const int innerCpNb = 9;
        static const int outerCpNb = 12;

        inline Entity GetInnerControlPoint(int index) const
            { return innerCp[index]; }

        inline Entity GetOuterControlPoint(int index) const
            { return outerCp[index]; }

    private:
        Entity innerCp[innerCpNb];
        Entity outerCp[outerCpNb];

        friend class GregoryPatchesSystem;
    };

    std::vector<Hole> FindHolesToFill(const std::vector<C0Patches>& patches) const;
};

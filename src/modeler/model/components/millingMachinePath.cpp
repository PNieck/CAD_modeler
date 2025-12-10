#include <CAD_modeler/model/components/millingMachinePath.hpp>


float MillingMachinePath::Length() const {
    float result = 0.f;

    for (size_t i = 1; i < commands.size(); i++) {
        const auto& prevPos = commands[i-1];
        const auto& curPos = commands[i];

        result += alg::Distance(
            prevPos.destination.vec,
            curPos.destination.vec
        );
    }

    return result;
}

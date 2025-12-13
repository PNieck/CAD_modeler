#include <CAD_modeler/model/millingPathsDesigner/millingMachinePathsBuilder.hpp>


void MillingMachinePathsBuilder::AddPosition(const Position &nextPosition)
{
    if (path.commands.empty()) {
        path.commands.emplace_back(nextID++, nextPosition);
        return;
    }

    const auto commandsCnt = path.commands.size();
    const auto& lastPos = path.commands[commandsCnt - 1].destination;

    if (lastPos.vec == nextPosition.vec)
        return;

    if (commandsCnt == 1) {
        path.commands.emplace_back(nextID++, nextPosition);
        return;
    }

    const auto& secondToLast = path.commands[commandsCnt-2].destination;

    alg::Vec3 a = lastPos.vec - secondToLast.vec;
    alg::Vec3 b = nextPosition.vec - lastPos.vec;

    a.NormalizeSelf();
    b.NormalizeSelf();

    if (1.f - alg::Dot(a, b) < eps) {
        path.commands.back().destination = nextPosition;
        return;
    }

    path.commands.emplace_back(nextID++, nextPosition);
}

void MillingMachinePathsBuilder::AddPositionFromOffset(const alg::Vec3 &offset)
{
    const auto& lastCommand = path.commands.back();
    auto newPos = lastCommand.destination.vec + offset;

    path.commands.emplace_back(nextID++, newPos);
}

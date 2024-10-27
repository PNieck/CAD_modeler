#pragma once

#include "../components/millingMachinePaths.hpp"

#include <string>


class MillingMachinePathsSystem {
public:
    static MillingMachinePaths ParseGCode(const std::string& filePath);
};

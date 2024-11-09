#pragma once

#include "../components/MillingMachinePath.hpp"
#include "../components/millingCutter.hpp"

#include <string>


class MillingMachinePathsSystem {
public:
    static MillingMachinePath ParseGCode(const std::string& filePath);
    static  MillingCutter ParseCutter(const std::string& filePath);

private:
    static constexpr float scale = 0.01f;
};

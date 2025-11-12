#pragma once

#include "../components/millingMachinePath.hpp"
#include "../components/millingCutter.hpp"

#include <string>
#include <string_view>


class MillingMachinePathsSystem {
public:
    static MillingMachinePath ParseGCode(const std::string& filePath);

    static MillingCutter ParseCutter(const std::string& filePath);

    static void CreateGCodeFile(const MillingMachinePath& paths, std::string_view path);

private:
    static constexpr float scale = 0.01f;
};

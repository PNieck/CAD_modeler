#include <CAD_modeler/model/systems/millingMachinePathsSystem.hpp>

#include <fstream>
#include <regex>
#include <cassert>
#include <filesystem>

namespace fs = std::filesystem;


MillingMachinePath MillingMachinePathsSystem::ParseGCode(const std::string &filePath)
{
    std::ifstream file;
    MillingMachinePath result;

    std::regex lineRegex(R"(^N(\d+)G01((X)(-?\d+\.\d+))?((Y)(-?\d+\.\d+))?((Z)(-?\d+\.\d+))?$)");

    file.open(filePath);

    std::string lineStr;
    while (std::getline(file, lineStr)) {
        std::smatch match;

        if (std::regex_match(lineStr, match, lineRegex)) {
            assert(match.size() == 11);

            int id;
            float x, y, z;
            bool coordinatesFound = false;

            if (match[1].str().empty())
                throw std::invalid_argument("Invalid GCode file");

            id = std::stoi(match[1].str());

            if (!match[3].str().empty()) {
                assert(match[3].str() == "X");
                x = std::stof(match[4].str());
                coordinatesFound = true;
            }
            else {
                x = result.commands.back().destination.GetX();
            }

            if (!match[6].str().empty()) {
                assert(match[6].str() == "Y");
                y = std::stof(match[7].str());
                coordinatesFound = true;
            }
            else {
                y = result.commands.back().destination.GetY();
            }

            if (!match[9].str().empty()) {
                assert(match[9].str() == "Z");
                z = std::stof(match[10].str());
                coordinatesFound = true;
            }
            else {
                z = result.commands.back().destination.GetZ();
            }

            if (!coordinatesFound)
                throw std::invalid_argument("Invalid GCode file");

            // Different coordinate systems conventions
            result.commands.emplace_back(id, y*scale, z*scale, x*scale);
        }
    }

    file.close();

    return result;
}


MillingCutter MillingMachinePathsSystem::ParseCutter(const std::string &filePath)
{
    const fs::path path(filePath);
    const std::regex extensionRegex(R"(^\.([kf])(\d+)$)");
    std::smatch match;

    const std::string str = path.extension().string();
    if (!std::regex_match(str, match, extensionRegex))
        throw std::invalid_argument("Invalid file");

    MillingCutter::Type cutterType;
    if (match[1].str() == "k")
        cutterType = MillingCutter::Type::Round;
    else if (match[1].str() == "f")
        cutterType = MillingCutter::Type::Flat;
    else
        throw std::invalid_argument("Invalid file");

    const float radius = static_cast<float>(std::stoi(match[2].str())) / 2.f * scale;

    return { radius, cutterType };
}

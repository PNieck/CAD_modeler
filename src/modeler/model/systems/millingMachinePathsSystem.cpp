#include <CAD_modeler/model/systems/millingMachinePathsSystem.hpp>

#include <fstream>
#include <regex>
#include <cassert>


MillingMachinePaths MillingMachinePathsSystem::ParseGCode(const std::string &filePath)
{
    std::ifstream file;
    MillingMachinePaths result;

    std::regex lineRegex(R"(^N(\d+)G01((X)(-?\d+\.\d+))?((Y)(-?\d+\.\d+))?((Z)(-?\d+\.\d+))?$)");

    // Ensure ifstream object can throw exceptions
    file.open(filePath);

    std::string lineStr;
    while (std::getline(file, lineStr)) {
        std::smatch match;

        if (std::regex_match(lineStr, match, lineRegex)) {
            assert(match.size() == 11);

            float x, y, z;
            bool coordinatesFound = false;

            if (!match[3].str().empty()) {
                assert(match[3].str() == "X");
                x = std::stof(match[4].str());
                coordinatesFound = true;
            }
            else {
                x = result.GetPositions().front().GetX();
            }

            if (!match[6].str().empty()) {
                assert(match[6].str() == "Y");
                y = std::stof(match[7].str());
                coordinatesFound = true;
            }
            else {
                y = result.GetPositions().front().GetY();
            }

            if (!match[9].str().empty()) {
                assert(match[9].str() == "Z");
                z = std::stof(match[10].str());
                coordinatesFound = true;
            }
            else {
                z = result.GetPositions().front().GetZ();
            }

            if (!coordinatesFound)
                throw std::runtime_error("Invalid GCode file");

            // Different coordinate systems conventions
            result.PushFront(Position{x/100.f, z/100.f, y/100.f});
        }
    }

    return result;
}

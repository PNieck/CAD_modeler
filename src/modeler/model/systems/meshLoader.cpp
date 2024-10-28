#include <CAD_modeler/model/systems/meshLoader.hpp>

#include <vector>
#include <fstream>
#include <regex>
#include <cassert>


MeshLoader::MeshLoader():
    vertexRegex(R"(^v +([+-]?([0-9]*[.])?[0-9]+) +([+-]?([0-9]*[.])?[0-9]+) +([+-]?([0-9]*[.])?[0-9]+)( +([+-]?([0-9]*[.])?[0-9]+))?$)"),
    faceRegex(R"(^f +(\d*)\/(\d*)\/(\d*) +(\d*)\/(\d*)\/(\d*) +(\d*)\/(\d*)\/(\d*)$)"),
    normalRegex(R"(^v +([+-]?([0-9]*[.])?[0-9]+) +([+-]?([0-9]*[.])?[0-9]+) +([+-]?([0-9]*[.])?[0-9]+)$)")
{
}


Mesh MeshLoader::LoadPositionsFromObj(const std::string &filepath) const
{
    Mesh result;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    std::ifstream file;

    file.open(filepath);

    std::string lineStr;
    while (std::getline(file, lineStr)) {
        std::smatch match;

        if (std::regex_match(lineStr, match, vertexRegex)) {
            assert(match.size() == 10);

            float x, y, z;
            x = std::stof(match[1].str());
            y = std::stof(match[3].str());
            z = std::stof(match[5].str());

            if (!match[8].str().empty()) {
                float w = std::stof(match[8].str());
                x /= w;
                y /= w;
                z /= w;
            }

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }


        else if (std::regex_match(lineStr, match, faceRegex)) {
            assert(match.size() == 10);

            indices.push_back(std::stoi(match[1].str()) - 1);
            indices.push_back(std::stoi(match[4].str()) - 1);
            indices.push_back(std::stoi(match[7].str()) - 1);
        }
    }

    file.close();

    result.Update(vertices, indices);

    return result;
}


// MeshWithNormals MeshLoader::LoadWithNormalsFromObj(const std::string &filepath)
// {
// }
//
//
// bool MeshLoader::TryParsePosition(const std::string &line, std::vector<float> &positions) const
// {
// }

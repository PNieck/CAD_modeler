#pragma once

#include "../components/mesh.hpp"
#include "../components/meshWithNormals.hpp"

#include <string>
#include <regex>


class MeshLoader {
public:
    MeshLoader();

    Mesh LoadPositionsFromObj(const std::string& filepath) const;

    MeshWithNormals LoadWithNormalsFromObj(const std::string& filepath);

private:
    std::regex vertexRegex;
    std::regex faceRegex;
    std::regex normalRegex;

};

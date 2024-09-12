#pragma once

#include <vector>
#include <cstdint>


class TorusParameters {
public:
    float majorRadius;
    float minorRadius;

    int meshDensityMinR;
    int meshDensityMajR;

    std::vector<float> GenerateVertices() const;
    std::vector<uint32_t> GenerateEdges() const;

private:
    // TODO: add comments
    float VertexX(float alpha, float beta) const;
    float VertexY(float alpha, float beta) const;
    float VertexZ(float alpha, float beta) const;
};

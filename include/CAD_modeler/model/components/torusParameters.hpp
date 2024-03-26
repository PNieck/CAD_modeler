#ifndef TORUS_PARAMETERS_H
#define TORUS_PARAMETERS_H

#include <vector>
#include <cstdint>


class TorusParameters {
public:
    float majorRadius;
    float minorRadius;

    // TODO: find better name
    int pointsInCirc;
    int circCnt;

    std::vector<float> GenerateVertices() const;
    std::vector<uint32_t> GenerateEdges() const;

private:
    // TODO: add comments
    float VertexX(float alpha, float beta) const;
    float VertexY(float alpha, float beta) const;
    float VertexZ(float alpha) const;
};


#endif

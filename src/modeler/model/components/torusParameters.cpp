#include <CAD_modeler/model/components/torusParameters.hpp>

#include <cmath>
#include <numbers>


std::vector<float> TorusParameters::GenerateVertices() const
{
    float deltaAlpha = 2.f * std::numbers::pi / meshDensityMinR;
    float deltaBeta = 2.f * std::numbers::pi / meshDensityMajR;

    std::vector<float> result(meshDensityMinR * meshDensityMajR * 3);

    for (int actCirc = 0; actCirc < meshDensityMajR; actCirc++) {
        float beta = deltaBeta * actCirc;

        for (int act_pt_in_circ = 0; act_pt_in_circ < meshDensityMinR; act_pt_in_circ++) {
            int index = (actCirc * meshDensityMinR + act_pt_in_circ) * 3;

            float alpha = deltaAlpha * act_pt_in_circ;

            result[index] = VertexX(alpha, beta);
            result[index + 1] = VertexY(alpha, beta);
            result[index + 2] = VertexZ(alpha);
        }
    }

    return result;
}


std::vector<uint32_t> TorusParameters::GenerateEdges() const
{
    std::vector<uint32_t> result(meshDensityMinR*meshDensityMajR + meshDensityMajR + meshDensityMinR*meshDensityMajR + meshDensityMinR);

    for (int i = 0; i < meshDensityMajR; i++) {
        for (int j=0; j < meshDensityMinR; j++) {
            result[i*meshDensityMinR + j + i] = j + i*meshDensityMinR;
        }

        result[meshDensityMinR * (i+1) + i] = std::numeric_limits<uint32_t>::max();
    }

    uint32_t index = meshDensityMinR * meshDensityMajR + meshDensityMajR;

    for (int i = 0; i < meshDensityMinR; i++) {
        for (int j=0; j < meshDensityMajR; j++) {
            result[index + j + i*meshDensityMajR + i] = j * meshDensityMinR + i;
        }

        result[index + meshDensityMajR*(i+1) + i] = std::numeric_limits<uint32_t>::max();
    }


    for (int i=0; i < meshDensityMajR; i++) {
        
    }

    return result;
}


float TorusParameters::VertexX(float alpha, float beta) const
{
    return (majorRadius + minorRadius * std::cosf(alpha)) * std::cos(beta);
}


float TorusParameters::VertexY(float alpha, float beta) const
{
    return (majorRadius + minorRadius * std::cosf(alpha)) * std::sin(beta);
}


float TorusParameters::VertexZ(float alpha) const
{
    return minorRadius * std::sin(alpha);
}

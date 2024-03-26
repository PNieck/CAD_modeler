#include <CAD_modeler/model/components/torusParameters.hpp>

#include <cmath>
#include <numbers>


std::vector<float> TorusParameters::GenerateVertices() const
{
    float deltaAlpha = 2 * std::numbers::pi / pointsInCirc;
    float deltaBeta = 2 * std::numbers::pi / circCnt;

    std::vector<float> result(pointsInCirc * circCnt * 3);

    for (int actCirc = 0; actCirc < circCnt; actCirc++) {
        float beta = deltaBeta * actCirc;

        for (int act_pt_in_circ = 0; act_pt_in_circ < pointsInCirc; act_pt_in_circ++) {
            int index = (actCirc * pointsInCirc + act_pt_in_circ) * 3;

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
    std::vector<uint32_t> result(pointsInCirc*circCnt + circCnt + pointsInCirc*circCnt + pointsInCirc);

    for (int i = 0; i < circCnt; i++) {
        for (int j=0; j < pointsInCirc; j++) {
            result[i*pointsInCirc + j + i] = j + i*pointsInCirc;
        }

        result[pointsInCirc * (i+1) + i] = std::numeric_limits<uint32_t>::max();
    }

    uint32_t index = pointsInCirc * circCnt + circCnt;

    for (int i = 0; i < pointsInCirc; i++) {
        for (int j=0; j < circCnt; j++) {
            result[index + j + i*circCnt + i] = j * pointsInCirc + i;
        }

        result[index + circCnt*(i+1) + i] = std::numeric_limits<uint32_t>::max();
    }


    for (int i=0; i < circCnt; i++) {
        
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

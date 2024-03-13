#include <CAD_modeler/objects/torus.hpp>

#include <cmath>
#include <numbers>

Torus::Torus(float r, float R):
    r(r), R(R)
{ }


std::vector<float> Torus::generate_vertices(int pts_in_circ, int circ_cnt) const
{
    float delta_alpha = 2 * std::numbers::pi / pts_in_circ;
    float delta_beta = 2 * std::numbers::pi / circ_cnt;

    std::vector<float> result(pts_in_circ * circ_cnt * 3);

    for (int act_circ = 0; act_circ < circ_cnt; act_circ++) {
        float beta = delta_beta * act_circ;

        for (int act_pt_in_circ = 0; act_pt_in_circ < pts_in_circ; act_pt_in_circ++) {
            int index = (act_circ * circ_cnt + act_pt_in_circ) * 3;

            float alpha = delta_alpha * act_pt_in_circ;

            result[index] = vertex_x(alpha, beta);
            result[index + 1] = vertex_y(alpha, beta);
            result[index + 2] = vertex_z(alpha);
        }
    }

    return result;
}


float Torus::vertex_x(float alpha, float beta) const
{
    return (R + r * std::cosf(alpha)) * std::cos(beta);
}


float Torus::vertex_y(float alpha, float beta) const
{
    return (R + r * std::cosf(alpha)) * std::sin(beta);
}


float Torus::vertex_z(float alpha) const
{
    return r * std::sin(alpha);
}

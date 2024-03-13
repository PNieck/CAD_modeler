#include <vector>

#include "object.hpp"

#include <glm/vec3.hpp>


class Torus : public Object {
public:
    Torus(float r, float R);

    std::vector<float> generate_vertices(int points_in_circ, int circ_cnt) const;

private:
    float r;
    float R;

    // TODO: add comments
    float vertex_x(float alpha, float beta) const;
    float vertex_y(float alpha, float beta) const;
    float vertex_z(float alpha) const;
};

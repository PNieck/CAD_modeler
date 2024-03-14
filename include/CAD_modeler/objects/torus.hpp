#include <vector>

#include "object.hpp"

#include <glm/vec3.hpp>
#include <glad/glad.h>


class Torus : public Object {
public:
    float r;
    float R;

    Torus(float r, float R);

    std::vector<float> generate_vertices(int points_in_circ, int circ_cnt) const;

    std::vector<GLuint> generate_edges(int points_in_circ, int circ_cnt) const;

private:
    // TODO: add comments
    float vertex_x(float alpha, float beta) const;
    float vertex_y(float alpha, float beta) const;
    float vertex_z(float alpha) const;
};

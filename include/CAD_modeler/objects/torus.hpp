#include <vector>

#include "object.hpp"

#include <glm/vec3.hpp>
#include <glad/glad.h>


class Torus : public Object {
public:
    Torus(float r, float R);

    inline void SmallRadiusSet(float newRadius) { r = newRadius; }
    inline float SmallRadiusGet() const { return r; }

    inline void BigRadiusSet(float newRadius) { R = newRadius; }
    inline float BigRadiusGet() const { return R; }

    std::vector<float> generate_vertices(int points_in_circ, int circ_cnt) const;

    std::vector<GLuint> generate_edges(int points_in_circ, int circ_cnt) const;

private:
    float r;
    float R;

    // TODO: add comments
    float vertex_x(float alpha, float beta) const;
    float vertex_y(float alpha, float beta) const;
    float vertex_z(float alpha) const;
};

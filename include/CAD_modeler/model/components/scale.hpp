#ifndef SCALE_H
#define SCALE_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>


class Scale {
public:
    Scale(float value = 1.0f): scale(value) {};
    Scale(float x, float y, float z): scale(x, y, z) {}

    glm::vec3 scale;

    glm::mat4x4 ScaleMatrix() const;
};


#endif

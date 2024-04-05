#ifndef SCALE_H
#define SCALE_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>


class Scale {
public:
    Scale(float value = 1.0f): scale(value) {};
    Scale(float x, float y, float z): scale(x, y, z) {}

    inline float GetX() const { return scale.x; }
    inline float GetY() const { return scale.y; } 
    inline float GetZ() const { return scale.z; }

    inline glm::vec3& AsVector() { return scale; }

    glm::mat4x4 ScaleMatrix() const;

    glm::vec3 scale;
private:
    
};


#endif

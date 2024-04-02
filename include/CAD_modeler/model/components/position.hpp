#ifndef POSITION_H
#define POSITION_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


class Position {
public:
    Position(): vec() {}
    Position(float val): vec(val) {}
    Position(const glm::vec3& vec): vec(vec) {}
    Position(float x, float y, float z): vec(x, y, z) {}

    glm::mat4x4 TranslationMatrix() const;

    inline float GetX() const { return vec.x; }
    inline float GetY() const { return vec.y; } 
    inline float GetZ() const { return vec.z; }

    inline void SetX(float x) { vec.x = x; }
    inline void SetY(float y) { vec.y = y; }
    inline void SetZ(float z) { vec.z = z; }

    glm::vec3 vec;
};


#endif

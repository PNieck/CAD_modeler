#ifndef POSITION_H
#define POSITION_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


class Position {
public:
    Position(): vec() {}
    Position(float val): vec(val) {}
    Position(float x, float y, float z): vec(x, y, z) {}

    glm::mat4x4 TranslationMatrix() const;

    inline float X() const { return vec.x; }
    inline float Y() const { return vec.y; } 
    inline float Z() const { return vec.z; } 

    glm::vec3 vec;
};


#endif

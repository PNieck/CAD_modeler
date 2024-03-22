#ifndef MODEL_H
#define MODEL_H

#include "camera.hpp"
#include "objects/torus.hpp"
#include "shader.hpp"


class Model
{
public:
    Model(int viewport_width, int viewport_height);

    void RenderFrame();

    inline void RotateTorusX(float angle) { torus.rotate_x(angle); }
    inline void RotateTorusY(float angle) { torus.rotate_y(angle); }
    inline void RotateTorusZ(float angle) { torus.rotate_z(angle); }

    inline void MoveTorusX(float offset) { torus.MoveX(offset); }
    inline void MoveTorusY(float offset) { torus.MoveY(offset); }
    inline void MoveTorusZ(float offset) { torus.MoveZ(offset); }

    inline float TorusScaleGet() const { return torus.ScaleGet(); }
    inline void TorusScaleSet(float newScale) { torus.ScaleSet(newScale); }

private:
    Camera camera;
    Torus torus;
    Shader shader;

    unsigned int VBO, VAO, EBO;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};


#endif
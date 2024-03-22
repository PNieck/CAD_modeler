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

    inline const Torus& ConstTorusGet() const { return torus; }
    inline Torus& TorusGet() { return torus; }

    void ChangeTorusBigRadius(float newRadius) {
        torus.BigRadiusSet(newRadius);

        vertices = torus.generate_vertices(4, 3);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        indices = torus.generate_edges(4, 3);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
    }

private:
    Camera camera;
    Torus torus;
    Shader shader;

    unsigned int VBO, VAO, EBO;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};


#endif
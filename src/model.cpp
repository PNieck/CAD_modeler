#include <CAD_modeler/model.hpp>


Model::Model(int viewport_width, int viewport_height):
    camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), (float)viewport_width/(float)viewport_height),
    torus(0.2f, 0.5f),
    shader("../../shaders/vertexShader.vert", "../../shaders/fragmentShader.frag")
{ }


void Model::Initialize()
{
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

    vertices = torus.generate_vertices(4, 2);
    indices = torus.generate_edges(4, 2);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * vertices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}


void Model::RenderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4x4 MVP = camera.GetPerspectiveMatrix() * camera.GetViewMatrix() * torus.model_matrix();
    shader.setMatrix4("MVP", MVP);

    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_LINE_LOOP, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

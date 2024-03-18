#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include <string>


class Shader {
public:
    unsigned int id;

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMatrix4(const std::string &name, const glm::mat4x4 &matrix) const;

private:
    int findUniformLocation(const std::string &name) const;
};


class UniformNotFoundInShader: std::exception {
public:
     const char * what() const override;
};


#endif

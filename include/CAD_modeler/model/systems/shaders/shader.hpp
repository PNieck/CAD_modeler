#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <string>


class Shader {
public:
    unsigned int id;

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    inline void use() const
        { glUseProgram(id); }

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMatrix4(const std::string &name, const glm::mat4x4 &matrix) const;
    void setVec4(const std::string& name, const glm::vec4& vec) const;

private:
    int findUniformLocation(const std::string &name) const;
};


class UniformNotFoundInShader: std::exception {
public:
     const char * what() const override;
};

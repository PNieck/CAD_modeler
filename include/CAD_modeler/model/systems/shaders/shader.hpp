#pragma once

#include <glad/glad.h>
#include <algebra/vec4.hpp>
#include <algebra/mat4x4.hpp>

#include <string>


class Shader {
public:
    unsigned int id;

    Shader(
        const char* vertexPath,
        const char* fragmentPath,
        const char* tesselationControlPath = nullptr,
        const char* tesselationEvaluationPath = nullptr,
        const char* geometryPath = nullptr
    );

    ~Shader();

    inline void use() const
        { glUseProgram(id); }

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMatrix4(const std::string &name, const alg::Mat4x4 &matrix) const;
    void setVec4(const std::string& name, const alg::Vec4& vec) const;

private:
    int findUniformLocation(const std::string &name) const;

    static unsigned int compileSingleShader(const char* path, GLenum shaderType);
};


class UniformNotFoundInShader: std::exception {
public:
     const char * what() const override;
};

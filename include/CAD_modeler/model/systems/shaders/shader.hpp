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

    void Use() const
        { glUseProgram(id); }

    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&) = delete;

protected:
    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetMatrix4(const std::string &name, const alg::Mat4x4 &matrix) const;
    void SetVec4(const std::string& name, const alg::Vec4& vec) const;
    void SetVec3(const std::string& name, const alg::Vec3& vec) const;


    constexpr static float MaxTessellationLevel = GL_MAX_TESS_GEN_LEVEL;

    void SetAllTesselationLevels(float level) const;

private:
    [[nodiscard]]
    int FindUniformLocation(const std::string &name) const;

    static unsigned int CompileSingleShader(const char* path, GLenum shaderType);
};


class UniformNotFoundInShader final : public std::exception {
public:
    [[nodiscard]]
    const char * what() const noexcept override;
};

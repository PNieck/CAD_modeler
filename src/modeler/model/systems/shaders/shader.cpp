#include <CAD_modeler/model/systems/shaders/shader.hpp>

#include <fstream>
#include <sstream>



Shader::Shader(
    const char* vertexPath,
    const char* fragmentPath,
    const char* tesselationControlPath,
    const char* tesselationEvaluationPath,
    const char* geometryPath
) {
    if (vertexPath == nullptr || fragmentPath == nullptr) {
        throw std::runtime_error("Vertex shader and fragment shader are mandatory");
    }

    if (tesselationControlPath != nullptr && tesselationEvaluationPath == nullptr) {
        throw std::runtime_error("Tesselation evaluation shader is required when tesselation control is used");
    }

    const unsigned int vertex = CompileSingleShader(vertexPath, GL_VERTEX_SHADER);
    const unsigned int fragment = CompileSingleShader(fragmentPath, GL_FRAGMENT_SHADER);
    unsigned int tesselationControl = 0;
    unsigned int tesselationEvaluation = 0;
    unsigned int geometry = 0;

    if (tesselationControlPath != nullptr)
        tesselationControl = CompileSingleShader(tesselationControlPath, GL_TESS_CONTROL_SHADER);

    if (tesselationEvaluationPath != nullptr)
        tesselationEvaluation = CompileSingleShader(tesselationEvaluationPath, GL_TESS_EVALUATION_SHADER);

    if (geometryPath != nullptr)
        geometry = CompileSingleShader(geometryPath, GL_GEOMETRY_SHADER);

    id = glCreateProgram();

    glAttachShader(id, vertex);
    glAttachShader(id, fragment);

    if (tesselationControlPath != nullptr)
        glAttachShader(id, tesselationControl);

    if (tesselationEvaluationPath != nullptr)
        glAttachShader(id, tesselationEvaluation);

    if (geometryPath != nullptr)
        glAttachShader(id, geometry);

    glLinkProgram(id);

    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success)
    {
        constexpr int infoLogSize = 512;
        char infoLog[infoLogSize];
        glGetProgramInfoLog(id, infoLogSize, nullptr, infoLog);
        throw std::runtime_error(infoLog);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (tesselationControlPath != nullptr)
        glDeleteShader(tesselationControl);
    
    if (tesselationEvaluationPath != nullptr)
        glDeleteShader(tesselationEvaluation);

    if (geometryPath != nullptr)
        glDeleteShader(geometry);
}


Shader::~Shader()
{
    glDeleteProgram(id);
}


void Shader::SetBool(const std::string &name, const bool value) const
{
    SetInt(name, (int)value);
}


void Shader::SetInt(const std::string & name, int value) const
{
    const int location = FindUniformLocation(name);
    glUniform1i(location, (int)value);
}


void Shader::SetFloat(const std::string & name, float value) const
{
    const int location = FindUniformLocation(name);
    glUniform1f(location, value);
}


void Shader::SetMatrix4(const std::string & name, const alg::Mat4x4& matrix) const
{
    const int location = FindUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.Data());
}


void Shader::SetVec4(const std::string & name, const alg::Vec4& vec) const
{
    const int location = FindUniformLocation(name);
    glUniform4fv(location, 1, vec.Data());
}


void Shader::SetVec3(const std::string &name, const alg::Vec3 &vec) const
{
    const int location = FindUniformLocation(name);
    glUniform3fv(location, 1, vec.Data());
}


int Shader::FindUniformLocation(const std::string & name) const
{
    const int location = glGetUniformLocation(id, name.c_str());

    if (location == -1) {
        throw UniformNotFoundInShader();
    }

    return location;
}


const char * UniformNotFoundInShader::what() const noexcept
{
    return "Cannot find uniform in shader";
}


unsigned int Shader::CompileSingleShader(const char* path, GLenum shaderType)
{
    std::ifstream shaderFile;

    // Ensure ifstream object can throw exceptions
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    shaderFile.open(path);

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    const std::string shaderCode = shaderStream.str();
    const char* shaderCodeCStr = shaderCode.c_str();

    const unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCodeCStr, nullptr);
    glCompileShader(shader);

    // Checking for compilation error
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        constexpr int infoLogSize = 512;
        char infoLog[infoLogSize];
        glGetShaderInfoLog(shader, infoLogSize, nullptr, infoLog);
        throw std::runtime_error(infoLog);
    }

    return shader;
}

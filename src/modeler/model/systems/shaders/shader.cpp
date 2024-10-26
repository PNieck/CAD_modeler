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

    unsigned int vertex = compileSingleShader(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragment = compileSingleShader(fragmentPath, GL_FRAGMENT_SHADER);
    unsigned int tesselationControl;
    unsigned int tesselationEvaluation;
    unsigned int geometry;

    if (tesselationControlPath != nullptr)
        tesselationControl = compileSingleShader(tesselationControlPath, GL_TESS_CONTROL_SHADER);

    if (tesselationEvaluationPath != nullptr)
        tesselationEvaluation = compileSingleShader(tesselationEvaluationPath, GL_TESS_EVALUATION_SHADER);

    if (geometryPath != nullptr)
        geometry = compileSingleShader(geometryPath, GL_GEOMETRY_SHADER);

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
        const int infoLogSize = 512;
        char infoLog[infoLogSize];
        glGetProgramInfoLog(id, infoLogSize, NULL, infoLog);
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


void Shader::setBool(const std::string &name, bool value) const
{
    setInt(name, (int)value);
}


void Shader::setInt(const std::string & name, int value) const
{
    int location = findUniformLocation(name);
    glUniform1i(location, (int)value);
}


void Shader::setFloat(const std::string & name, float value) const
{
    int location = findUniformLocation(name);
    glUniform1f(location, value);
}


void Shader::setMatrix4(const std::string & name, const alg::Mat4x4& matrix) const
{
    int location = findUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.Data());
}


void Shader::setVec4(const std::string & name, const alg::Vec4& vec) const
{
    int location = findUniformLocation(name);
    glUniform4fv(location, 1, vec.Data());
}


int Shader::findUniformLocation(const std::string & name) const
{
    int location = glGetUniformLocation(id, name.c_str());

    if (location == -1) {
        throw UniformNotFoundInShader();
    }

    return location;
}


const char * UniformNotFoundInShader::what() const
{
    return "Cannot find uniform in shader";
}


unsigned int Shader::compileSingleShader(const char* path, GLenum shaderType)
{
    const int infoLogSize = 512;
    char infoLog[infoLogSize];

    std::ifstream shaderFile;

    // Ensure ifstream object can throw exceptions
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    shaderFile.open(path);

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    std::string shaderCode = shaderStream.str();
    const char* shaderCodeCStr = shaderCode.c_str();

    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCodeCStr, NULL);
    glCompileShader(shader);

    // Checking for compilation error
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, infoLogSize, NULL, infoLog);
        throw std::runtime_error(infoLog);
    }

    return shader;
}

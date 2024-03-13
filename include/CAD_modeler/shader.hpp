#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

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
};


#endif

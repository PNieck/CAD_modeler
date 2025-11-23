#pragma once

#include <string_view>

#include "glad/glad.h"


// OpenGL help
namespace glh {
    void SaveColorBufferToPPM(std::string_view path);

    void SaveTextureToPPM(GLuint texture, int width, int height, std::string_view path);
}

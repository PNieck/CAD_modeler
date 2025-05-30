#pragma once

#include <glad/glad.h>


class Texture2D {
public:
    enum InternalFormat {
        Red32BitFloat = GL_R32F,
        Red8BitFloat = GL_R8
    };

    enum InputDataFormat {
        Red = GL_RED,
    };

    Texture2D(int width, int height, const float *data, InternalFormat internalFormat, InputDataFormat inputFormat);
    // Texture2D(const Texture2D &texture) = delete;
    // ~Texture2D();

    void Update(const float *data, InputDataFormat inputFormat) const;

    void ChangeSize(int texWidth, int texHeight, const float *data, InputDataFormat inputFormat);

    void Use() const
        { glBindTexture(GL_TEXTURE_2D, id); }

    [[nodiscard]]
    int GetWidth() const
        { return width; }

    [[nodiscard]]
    int GetHeight() const
        { return height; }

    [[nodiscard]]
    unsigned int Id() const
        { return id; }

private:
    unsigned int id = 0;
    int width;
    int height;
    InternalFormat internalFormat;
};

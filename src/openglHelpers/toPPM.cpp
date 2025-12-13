#include <openglHelpers/toPPM.hpp>

#include <glad/glad.h>

#include <vector>
#include <tuple>
#include <fstream>
#include <stdexcept>


std::tuple<int, int> GetViewportSize()
{
    GLint viewportSize[4];
    glGetIntegerv(GL_VIEWPORT, viewportSize);

    return std::make_tuple(viewportSize[2], viewportSize[3]);
}


void glh::SaveColorBufferToPPM(std::string_view path)
{
    // Make sure alignment won't screw us ( tightly packed RGB )
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glFinish(); // ensure all drawing finished

    auto [width, height] = GetViewportSize();
    std::vector<unsigned char> pixels(3 * width * height);
    // Read pixels from lower-left corner (OpenGL origin)
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    FILE* f = std::fopen(path.data(), "wb");
    if (!f)
        throw std::runtime_error("Failed to open file for writing");

    // PPM header (P6 = binary)
    std::fprintf(f, "P6\n%d %d\n255\n", width, height);

    // OpenGL origin is lower-left; many image viewers expect row-major
    // top-to-bottom. We flip rows here while writing.
    const size_t rowBytes = static_cast<size_t>(width) * 3;
    for (int y = height - 1; y >= 0; --y) {
        unsigned char* rowStart = pixels.data() + static_cast<size_t>(y) * rowBytes;
        if (std::fwrite(rowStart, 1, rowBytes, f) != rowBytes) {
            std::fclose(f);
            throw std::runtime_error("Write error");
        }
    }

    std::fclose(f);
}


void glh::SaveTextureToPPM(GLuint texture, int width, int height, std::string_view path)
{
    // Bind texture for reading
    glBindTexture(GL_TEXTURE_2D, texture);

    // Allocate buffer: RGB (3 bytes per pixel)
    std::vector<unsigned char> pixels(width * height * 3);

    // Read pixels from texture
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    std::ofstream out(path.data());
    if (!out) return;

    // Write PPM header (P3: ASCII)
    out << "P3\n" << width << " " << height << "\n255\n";

    // Write pixel data in PPM top-left to bottom-right order
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            int i = 3 * (y * width + x);
            out << static_cast<int>(pixels[i]) << " "
                << static_cast<int>(pixels[i + 1]) << " "
                << static_cast<int>(pixels[i + 2]) << " ";
        }
        out << "\n";
    }
}

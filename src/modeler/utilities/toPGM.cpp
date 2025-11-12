#include <CAD_modeler/utilities/toPGM.hpp>

#include <stdexcept>
#include <fstream>


void ToPGM(const FlatVec2D<float> &data, std::string_view path)
{

    size_t width = data.Rows();
    size_t height = data.Cols();

    // Normalize depth to [0, 255] for visualization
    std::vector<unsigned char> image(width * height);
    for (int i = 0; i < width * height; ++i) {
        float d = *(data.Data() + i);
        // clamp to [0, 1] to be safe
        d = std::max(0.0f, std::min(1.0f, d));
        image[i] = static_cast<unsigned char>(d * 255.0f);
    }

    // Write as binary PGM (P5)
    std::ofstream file(path.data(), std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file");

    // PGM header
    file << "P5\n" << width << " " << height << "\n255\n";

    // OpenGL origin is bottom-left → flip vertically for typical image orientation
    for (ssize_t y = height - 1; y >= 0; --y) {
        file.write(reinterpret_cast<const char*>(&image[y * width]), width);
    }

    file.close();
}

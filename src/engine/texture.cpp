#include "texture.h"
#include "texture_loader.h"
#include <SOIL2.h>

bool Texture::ComputeAverageColorFromFile(const std::string& path, glm::vec3& out_average_rgb)
{
    int w = 0, h = 0, channels = 0;
    unsigned char* data = TextureLoader::LoadImagePixels(path, w, h, channels);
    if (!data || w <= 0 || h <= 0)
    {
        if (data) SOIL_free_image_data(data);
        return false;
    }

    const int stride = channels > 0 ? channels : 3;
    double r = 0.0, g = 0.0, b = 0.0;
    const size_t pixel_count = static_cast<size_t>(w) * static_cast<size_t>(h);

    for (int y = 0; y < h; ++y)
    {
        const unsigned char* row = data + static_cast<size_t>(y) * static_cast<size_t>(w) * stride;
        for (int x = 0; x < w; ++x)
        {
            const unsigned char* px = row + static_cast<size_t>(x) * stride;
            // If grayscale, replicate to rgb
            const unsigned char r8 = px[0];
            const unsigned char g8 = (stride >= 3) ? px[1] : px[0];
            const unsigned char b8 = (stride >= 3) ? px[2] : px[0];
            r += r8;
            g += g8;
            b += b8;
        }
    }

    const double inv = 1.0 / (255.0 * static_cast<double>(pixel_count));
    out_average_rgb = glm::vec3(static_cast<float>(r * inv),
                                static_cast<float>(g * inv),
                                static_cast<float>(b * inv));
    SOIL_free_image_data(data);
    return true;
}



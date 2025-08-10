#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
class Texture;

// Simple helper for loading 2D textures into OpenGL using SOIL2
class TextureLoader
{
public:
    static void LoadTexture2DFromFile(const std::string& path,
                                      bool generate_mipmaps,
                                      Texture& texture);

private:
    static void LoadTexture2DFromFile(const std::string& path, bool generate_mipmaps, GLuint& tex_id);
    friend class Texture;
    // Helper used by Texture to compute average color without duplicating image loading code.
    // Loads the image data (SOIL2), returns width, height, channels and pointer to data.
    // Caller is responsible for freeing data with SOIL_free_image_data.
    static unsigned char* LoadImagePixels(const std::string& path,
                                          int& out_width,
                                          int& out_height,
                                          int& out_channels);
};



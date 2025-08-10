#pragma once

#include <string>
#include <glad/glad.h>
class Texture;

// Simple helper for loading 2D textures into OpenGL using SOIL2
class TextureLoader
{
public:
    static void LoadTexture2DFromFile(const std::string& path,
                                      bool generate_mipmaps,
                                      Texture& texture);

private:
    static void loadTexture2DFromFile(const std::string& path, bool generate_mipmaps, GLuint& tex_id);
};



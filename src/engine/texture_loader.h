#pragma once

#include <string>
#include <glad/glad.h>

// Simple helper for loading 2D textures into OpenGL using SOIL2
class TextureLoader
{
public:
    // Loads an image from disk into a GL texture (GL_TEXTURE_2D)
    // Returns texture id (non-zero). On failure, throws std::runtime_error.
    // If generate_mipmaps is true, creates mipmaps and sets MIN filter accordingly.
    static void LoadTexture2DFromFile(const std::string& path,
                                        bool generate_mipmaps , GLuint& tex_id);
};



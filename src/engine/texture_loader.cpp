#include "texture_loader.h"

#include <SOIL2.h>
#include <stdexcept>
#include "engine/texture.h"

void TextureLoader::LoadTexture2DFromFile(const std::string& path, bool generate_mipmaps, GLuint& tex_id)
{
    int width = 0, height = 0, channels = 0;
    unsigned char* data = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
    if (!data)
    {
        throw std::runtime_error(std::string("SOIL2 failed to load: ") + path + ": " + SOIL_last_result());
    }

    GLuint tex = 0;
    if (tex_id == 0) {
        glGenTextures(1, &tex);
    } else {
        tex = tex_id;
    }
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    generate_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLenum format = GL_RGB;
    if (channels == 4) format = GL_RGBA;
    else if (channels == 1) format = GL_RED;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    if (generate_mipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    SOIL_free_image_data(data);
    tex_id = tex;
}

void TextureLoader::LoadTexture2DFromFile(const std::string& path, bool generate_mipmaps, Texture& texture)
{
    GLuint id = texture.id();
    LoadTexture2DFromFile(path, generate_mipmaps, id);
    texture.reset(id);
}

unsigned char* TextureLoader::LoadImagePixels(const std::string& path,
                                              int& out_width,
                                              int& out_height,
                                              int& out_channels)
{
    out_width = out_height = out_channels = 0;
    unsigned char* data = SOIL_load_image(path.c_str(), &out_width, &out_height, &out_channels, SOIL_LOAD_AUTO);
    return data;
}



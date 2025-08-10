#include "material.h"
#include "shader.h"
#include "texture.h"
#include "texture_loader.h"

#include <utility>

bool Material::EnsureResourcesLoaded()
{
    // Compile shader if not compiled yet and paths provided
    if (!shader_ && !vertex_shader_path.empty() && !fragment_shader_path.empty())
    {
        try
        {
            shader_ = std::make_shared<Shader>(Shader::FromFiles(vertex_shader_path, fragment_shader_path));
        }
        catch (...)
        {
            shader_.reset();
            return false;
        }
    }

    // Load albedo texture if a path is specified and not loaded yet
    if (!albedo_texture_path.empty() && !albedo_texture_)
    {
        auto tex = std::make_shared<Texture>();
        try
        {
            TextureLoader::LoadTexture2DFromFile(albedo_texture_path, false, *tex);
            albedo_texture_ = std::move(tex);
        }
        catch (...)
        {
            albedo_texture_.reset();
            // Not fatal; material can still render using color only
        }
    }

    return static_cast<bool>(shader_);
}



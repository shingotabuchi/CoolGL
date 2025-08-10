#pragma once

#include "component.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include <memory>

// Renders an equirectangular sky texture as a background using a unit cube.
// Sets appropriate depth/culling state so it appears behind all geometry.
class SkyboxRenderer : public Component
{
public:
    SkyboxRenderer() = default;

    void SetTexture(std::shared_ptr<Texture> texture) { texture_ = std::move(texture); }

    void OnRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view) override;

    std::unique_ptr<Component> Clone() const override
    {
        auto copy = std::make_unique<SkyboxRenderer>();
        copy->texture_ = texture_;
        return copy;
    }

private:
    static std::shared_ptr<Mesh> GetOrCreateCubeMesh();
    void EnsureShader();

private:
    std::shared_ptr<Texture> texture_{};
    Shader shader_{};
};



#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <string>
#include <glad/glad.h>
#include "shader.h"

class GameObject;
class Renderer;
class Camera;
class Light;

class Scene
{
public:
    Scene() = default;

    GameObject& CreateObject();
    void Update(float time_seconds);
    void Render(Renderer& renderer);

    // Camera management
    void RegisterCamera(Camera* camera);
    void UnregisterCamera(Camera* camera);
    Camera* GetActiveCamera() const { return active_camera_; }

    // Light management (supports up to Light::kMaxLights)
    void RegisterLight(Light* light);
    void UnregisterLight(Light* light);
    const std::vector<Light*>& GetLights() const { return lights_; }

    // Ambient light management (in RGB, linear space)
    void SetAmbientColor(const glm::vec3& c) { ambient_color_ = c; }
    glm::vec3 GetAmbientColor() const { return ambient_color_; }

    // Sky management (equirectangular 2D texture interpreted as sky)
    // Loads the image, computes average color (for ambient), and stores a clear color
    // derived from horizon average for frame clear. Returns false on failure.
    bool SetSkyFromEquirect(const std::string& path);
    glm::vec3 GetSkyClearColor() const { return sky_clear_color_; }

private:
    // Important: Declare manager containers before `objects_` so they
    // outlive `objects_` during destruction. Components' OnDetach may
    // reference these containers (e.g., lights), so they must still be valid
    // while GameObjects are being destroyed.
    Camera* active_camera_ = nullptr;
    std::vector<Light*> lights_{};
    std::vector<std::unique_ptr<GameObject>> objects_;
    glm::vec3 ambient_color_{0.0f, 0.0f, 0.0f};
    glm::vec3 sky_clear_color_{0.1f, 0.2f, 0.3f};

    // Sky rendering resources (optional). Only valid if sky_texture_ != 0.
    GLuint sky_texture_ = 0;
    GLuint sky_vao_ = 0;
    GLuint sky_vbo_ = 0;
    GLuint sky_ebo_ = 0;
    Shader sky_shader_{};

    // Internal helpers
    void RenderSky(const glm::mat4& projection, const glm::mat4& view);
    void EnsureSkyMeshAndShader();
};



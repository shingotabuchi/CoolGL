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
    // Duplicate a GameObject with supported components (Transform, Camera, Light)
    // Note: MeshRenderer is not duplicated due to GPU resource ownership; attach a new one manually.
    GameObject& Instantiate(const GameObject& original);
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
    glm::vec3 GetClearColor() const { return clear_color_; }

private:
    // Important: Declare manager containers before `objects_` so they
    // outlive `objects_` during destruction. Components' OnDetach may
    // reference these containers (e.g., lights), so they must still be valid
    // while GameObjects are being destroyed.
    Camera* active_camera_ = nullptr;
    std::vector<Light*> lights_{};
    std::vector<std::unique_ptr<GameObject>> objects_;
    glm::vec3 ambient_color_{0.0f, 0.0f, 0.0f};
    glm::vec3 clear_color_{0.1f, 0.2f, 0.3f};

};



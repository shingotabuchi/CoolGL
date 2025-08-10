#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

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

private:
    // Important: Declare manager containers before `objects_` so they
    // outlive `objects_` during destruction. Components' OnDetach may
    // reference these containers (e.g., lights), so they must still be valid
    // while GameObjects are being destroyed.
    Camera* active_camera_ = nullptr;
    std::vector<Light*> lights_{};
    std::vector<std::unique_ptr<GameObject>> objects_;
};



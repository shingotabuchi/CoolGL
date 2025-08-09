#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

class GameObject;
class Renderer;
class Camera;

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

private:
    std::vector<std::unique_ptr<GameObject>> objects_;
    Camera* active_camera_ = nullptr;
};



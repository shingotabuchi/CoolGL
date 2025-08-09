#include "scene.h"
#include "game_object.h"
#include "camera.h"

GameObject& Scene::CreateObject()
{
    auto obj = std::make_unique<GameObject>();
    GameObject& ref = *obj;
    ref.SetScene(this);
    objects_.push_back(std::move(obj));
    return ref;
}

void Scene::Update(float time_seconds)
{
    for (auto& obj : objects_)
    {
        obj->Update(time_seconds);
    }
}

void Scene::Render(Renderer& renderer)
{
    const Camera* activeCamera = active_camera_;
    if (!activeCamera)
    {
        // No camera: skip rendering to enforce 'spawn a camera to render'
        return;
    }

    const glm::mat4 projection = activeCamera->ProjectionMatrix();
    const glm::mat4 view = activeCamera->ViewMatrix();

    for (auto& obj : objects_)
    {
        obj->Render(renderer, projection, view);
    }
}

void Scene::RegisterCamera(Camera* camera)
{
    // First-come-first-serve; allow swapping by explicit unregister/register order
    if (!active_camera_)
    {
        active_camera_ = camera;
    }
}

void Scene::UnregisterCamera(Camera* camera)
{
    if (active_camera_ == camera)
    {
        active_camera_ = nullptr;
    }
}



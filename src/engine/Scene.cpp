#include "scene.h"
#include "game_object.h"
#include "camera.h"
#include "light.h"
#include "renderer.h"
#include "transform.h"
#include "texture_loader.h"
#include "texture.h"
#include "skybox_renderer.h"

#include <SOIL2.h>

GameObject& Scene::CreateObject()
{
    auto obj = std::make_unique<GameObject>();
    GameObject& ref = *obj;
    ref.SetScene(this);
    objects_.push_back(std::move(obj));
    return ref;
}

GameObject& Scene::Instantiate(const GameObject& original)
{
    GameObject& clone = CreateObject();
    original.CloneComponentsTo(clone);
    return clone;
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

    // Clear using sky color so sky acts as background
    renderer.BeginFrame(clear_color_.r, clear_color_.g, clear_color_.b, 1.0f);

    // Skybox will render like any other GameObject via its own component

    for (auto& obj : objects_)
    {
        obj->Render(renderer, projection, view);
    }
}

bool Scene::SetSkyFromEquirect(const std::string& path)
{
    // Load GL texture into a temporary wrapper
    Texture sky_texture;
    TextureLoader::LoadTexture2DFromFile(path, false, sky_texture);

    // Compute average color for ambient
    glm::vec3 avg;
    if (Texture::ComputeAverageColorFromFile(path, avg))
    {
        ambient_color_ = avg * 0.8f;
    }

    // Create or update a dedicated Sky object with SkyboxRenderer
    GameObject& sky = CreateObject();
    auto* skybox = sky.AddComponent<SkyboxRenderer>();
    skybox->SetTexture(std::make_shared<Texture>(sky_texture.id()));
    return true;
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

void Scene::RegisterLight(Light* light)
{
    // Avoid duplicates
    for (auto* l : lights_)
    {
        if (l == light) return;
    }
    lights_.push_back(light);
}

void Scene::UnregisterLight(Light* light)
{
    lights_.erase(std::remove(lights_.begin(), lights_.end(), light), lights_.end());
}



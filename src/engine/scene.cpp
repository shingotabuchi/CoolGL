#include "scene.h"
#include "game_object.h"
#include "camera.h"
#include "light.h"
#include "renderer.h"
#include "transform.h"
#include "texture_loader.h"
#include "texture.h"
#include "mesh_renderer.h"

#include <SOIL2.h>

GameObject &Scene::CreateObject()
{
    auto obj = std::make_unique<GameObject>();
    GameObject &ref = *obj;
    ref.SetScene(this);
    objects_.push_back(std::move(obj));
    return ref;
}

GameObject &Scene::Instantiate(const GameObject &original)
{
    GameObject &clone = CreateObject();
    original.CloneComponentsTo(clone);
    return clone;
}

void Scene::Update(float time_seconds)
{
    for (auto &obj : objects_)
    {
        obj->Update(time_seconds);
    }
}

void Scene::Render(Renderer &renderer)
{
    const Camera *activeCamera = active_camera_;
    if (!activeCamera)
    {
        // No camera: skip rendering to enforce 'spawn a camera to render'
        return;
    }

    const glm::mat4 projection = activeCamera->ProjectionMatrix();
    const glm::mat4 view = activeCamera->ViewMatrix();
    const glm::vec3 cam_pos = activeCamera->Position();

    // Fetch lights from scene and transform their world directions into object space
    glm::vec3 light_dirs[Light::kMaxLights];
    glm::vec3 light_colors[Light::kMaxLights];
    int light_count = 0;
    const auto &lights = GetLights();
    for (size_t i = 0; i < lights.size() && light_count < Light::kMaxLights; ++i)
    {
        const Light *light = lights[i];
        if (!light)
            continue;
        light_dirs[light_count] = glm::normalize(light->WorldDirection());
        light_colors[light_count] = light->color * light->intensity;
        ++light_count;
    }
    renderer.UpdateLightState(light_count, light_dirs, light_colors, cam_pos);

    // Clear using sky color so sky acts as background
    renderer.BeginFrame(clear_color_.r, clear_color_.g, clear_color_.b, 1.0f);

    // Render skybox first (if any)
    if (skybox_object_)
    {
        skybox_object_->Render(renderer, projection, view);
    }

    for (auto &obj : objects_)
    {
        obj->Render(renderer, projection, view);
    }

    // renderer.DrawInstanced(projection, view);
}

bool Scene::SetSkyFromEquirect(const std::string &path)
{
    // Load GL texture into a persistent shared Texture
    auto sky_tex = std::make_shared<Texture>();
    TextureLoader::LoadTexture2DFromFile(path, false, *sky_tex);

    // Compute average color for ambient
    glm::vec3 avg;
    if (Texture::ComputeAverageColorFromFile(path, avg))
    {
        // ambient_color_ = avg * 0.8f;
        ambient_color_ = avg;
    }

    // Create skybox object with MeshRenderer in Skybox mode
    if (!skybox_object_)
    {
        skybox_object_ = std::make_unique<GameObject>();
        skybox_object_->SetScene(this);
    }
    MeshRenderer *mr = skybox_object_->GetComponent<MeshRenderer>();
    if (!mr)
    {
        mr = skybox_object_->AddComponent<MeshRenderer>();
        mr->render_mode = MeshRenderer::RenderMode::Skybox;
        mr->SetMesh(MeshRenderer::CreateUnitCube());
    }
    mr->diffuse_texture = std::move(sky_tex);
    return true;
}

void Scene::RegisterCamera(Camera *camera)
{
    // First-come-first-serve; allow swapping by explicit unregister/register order
    if (!active_camera_)
    {
        active_camera_ = camera;
    }
}

void Scene::UnregisterCamera(Camera *camera)
{
    if (active_camera_ == camera)
    {
        active_camera_ = nullptr;
    }
}

void Scene::RegisterLight(Light *light)
{
    // Avoid duplicates
    for (auto *l : lights_)
    {
        if (l == light)
            return;
    }
    lights_.push_back(light);
}

void Scene::UnregisterLight(Light *light)
{
    lights_.erase(std::remove(lights_.begin(), lights_.end(), light), lights_.end());
}

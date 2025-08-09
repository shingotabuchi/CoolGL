#include "scene.h"
#include "game_object.h"

GameObject& Scene::CreateObject()
{
    auto obj = std::make_unique<GameObject>();
    GameObject& ref = *obj;
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

void Scene::Render(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view)
{
    for (auto& obj : objects_)
    {
        obj->Render(renderer, projection, view);
    }
}



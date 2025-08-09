#include "Scene.h"
#include "GameObject.h"

GameObject& Scene::createObject()
{
    auto obj = std::make_unique<GameObject>();
    GameObject& ref = *obj;
    objects_.push_back(std::move(obj));
    return ref;
}

void Scene::update(float timeSeconds)
{
    for (auto& obj : objects_)
    {
        obj->update(timeSeconds);
    }
}

void Scene::render(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view)
{
    for (auto& obj : objects_)
    {
        obj->render(renderer, projection, view);
    }
}



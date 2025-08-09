#include "GameObject.h"
#include "Component.h"

GameObject::~GameObject()
{
    for (auto& c : components_)
    {
        c->onDetach();
    }
}

void GameObject::update(float timeSeconds)
{
    for (auto& c : components_)
    {
        auto typeId = std::type_index(typeid(*c));
        if (!started_[typeId])
        {
            c->onStart();
            started_[typeId] = true;
        }
        c->onUpdate(timeSeconds);
    }
}

void GameObject::render(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view)
{
    for (auto& c : components_)
    {
        c->onRender(renderer, projection, view);
    }
}



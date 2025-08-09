#include "game_object.h"

GameObject::~GameObject()
{
    for (auto& c : components_)
    {
        c->OnDetach();
    }
}

void GameObject::Update(float time_seconds)
{
    for (auto& c : components_)
    {
        auto type_id = std::type_index(typeid(*c));
        if (!started_[type_id])
        {
            c->OnStart();
            started_[type_id] = true;
        }
        c->OnUpdate(time_seconds);
    }
}

void GameObject::Render(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view)
{
    for (auto& c : components_)
    {
        c->OnRender(renderer, projection, view);
    }
}



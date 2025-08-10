#include "game_object.h"
#include "component.h"

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
        if (!c->IsStarted())
        {
            c->OnStart();
            c->MarkStarted();
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

void GameObject::AddExistingComponent(std::unique_ptr<Component> comp)
{
    if (!comp) return;
    comp->SetOwner(this);
    Component* raw = comp.get();
    components_.push_back(std::move(comp));
    const std::type_index typeId = std::type_index(typeid(*raw));
    if (component_cache_.find(typeId) == component_cache_.end())
    {
        component_cache_[typeId] = raw;
    }
    raw->OnAttach();
}

void GameObject::CloneComponentsTo(GameObject& target) const
{
    for (const auto& c : components_)
    {
        if (!c) continue;
        std::unique_ptr<Component> cloned = c->Clone();
        if (cloned)
        {
            target.AddExistingComponent(std::move(cloned));
        }
    }
}



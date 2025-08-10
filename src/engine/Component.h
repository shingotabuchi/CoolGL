#pragma once

#include <glm/glm.hpp>
#include <memory>

class GameObject;
class Renderer;

class Component
{
public:
    virtual ~Component() = default;

    void SetOwner(GameObject* owner) { owner_ = owner; }
    GameObject* Owner() const { return owner_; }

    // Lifecycle helpers for performance and correctness
    bool IsStarted() const { return started_; }
    void MarkStarted() { started_ = true; }

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnStart() {}
    virtual void OnUpdate(float time_seconds) {}
    virtual void OnRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view) {}

    // Polymorphic clone used by Scene::Instantiate to duplicate components
    // Default returns nullptr (component not cloneable); override in derived classes
    virtual std::unique_ptr<Component> Clone() const { return nullptr; }

protected:
    GameObject* owner_ = nullptr;
    bool started_ = false;
};



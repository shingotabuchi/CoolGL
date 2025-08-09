#pragma once

#include <glm/glm.hpp>

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

protected:
    GameObject* owner_ = nullptr;
    bool started_ = false;
};



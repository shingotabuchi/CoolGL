#pragma once

#include <glm/glm.hpp>

class GameObject;
class Renderer;

class Component
{
public:
    virtual ~Component() = default;

    void setOwner(GameObject* owner) { owner_ = owner; }
    GameObject* owner() const { return owner_; }

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onStart() {}
    virtual void onUpdate(float timeSeconds) {}
    virtual void onRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view) {}

protected:
    GameObject* owner_ = nullptr;
};



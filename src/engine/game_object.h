#pragma once

#include <vector>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <glm/glm.hpp>
#include "component.h"

class Renderer;

class GameObject
{
public:
    GameObject() = default;
    ~GameObject();

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->SetOwner(this);
        T* raw = comp.get();
        components_.push_back(std::move(comp));
        started_[std::type_index(typeid(T))] = false;
        raw->OnAttach();
        return raw;
    }

    template<typename T>
    T* GetComponent()
    {
        for (auto& c : components_)
        {
            if (auto casted = dynamic_cast<T*>(c.get()))
            {
                return casted;
            }
        }
        return nullptr;
    }

    void Update(float time_seconds);
    void Render(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view);

private:
    std::vector<std::unique_ptr<Component>> components_;
    std::unordered_map<std::type_index, bool> started_;
};



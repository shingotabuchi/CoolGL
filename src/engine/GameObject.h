#pragma once

#include <vector>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Component.h"
class Renderer;

class GameObject
{
public:
    GameObject() = default;
    ~GameObject();

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    template<typename T, typename... Args>
    T* addComponent(Args&&... args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->setOwner(this);
        T* raw = comp.get();
        components_.push_back(std::move(comp));
        started_[std::type_index(typeid(T))] = false;
        raw->onAttach();
        return raw;
    }

    template<typename T>
    T* getComponent()
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

    void update(float timeSeconds);
    void render(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view);

private:
    std::vector<std::unique_ptr<Component>> components_;
    std::unordered_map<std::type_index, bool> started_;
};



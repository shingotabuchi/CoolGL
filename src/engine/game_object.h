#pragma once

#include <vector>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <glm/glm.hpp>
#include "component.h"

class Renderer;
class Scene;

class GameObject
{
public:
    GameObject() = default;
    ~GameObject();

    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;

    template <typename T, typename... Args>
    T *AddComponent(Args &&...args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->SetOwner(this);
        T *raw = comp.get();
        components_.push_back(std::move(comp));
        // Cache first component of a given type for fast lookup
        const std::type_index typeId = std::type_index(typeid(T));
        if (component_cache_.find(typeId) == component_cache_.end())
        {
            component_cache_[typeId] = raw;
        }
        raw->OnAttach();
        return raw;
    }

    template <typename T>
    T *GetComponent()
    {
        const std::type_index typeId = std::type_index(typeid(T));
        auto it = component_cache_.find(typeId);
        if (it != component_cache_.end())
        {
            return static_cast<T *>(it->second);
        }
        // Fallback: scan and populate cache for next time
        for (auto &c : components_)
        {
            if (auto casted = dynamic_cast<T *>(c.get()))
            {
                component_cache_[typeId] = casted;
                return casted;
            }
        }
        return nullptr;
    }

    template <typename T>
    const T *GetComponent() const
    {
        // Note: const overload does not populate the cache to preserve constness
        for (const auto &c : components_)
        {
            if (auto casted = dynamic_cast<const T *>(c.get()))
            {
                return casted;
            }
        }
        return nullptr;
    }

    void Update(float time_seconds);
    void Render(Renderer &renderer, const glm::mat4 &projection, const glm::mat4 &view);

    // Scene access
    void SetScene(Scene *scene) { scene_ = scene; }
    Scene *GetScene() const { return scene_; }

    // Internal helpers used by cloning logic
    void AddExistingComponent(std::unique_ptr<Component> comp);
    void CloneComponentsTo(GameObject &target) const;

private:
    std::vector<std::unique_ptr<Component>> components_;
    std::unordered_map<std::type_index, Component *> component_cache_;
    Scene *scene_ = nullptr;
};

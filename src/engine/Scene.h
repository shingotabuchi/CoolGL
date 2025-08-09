#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

class GameObject;
class Renderer;

class Scene
{
public:
    Scene() = default;

    GameObject& createObject();
    void update(float timeSeconds);
    void render(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view);

private:
    std::vector<std::unique_ptr<GameObject>> objects_;
};



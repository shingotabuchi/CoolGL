#pragma once

#include <memory>
#include <glm/glm.hpp>

class Window;

class Application
{
public:
    Application(int width, int height, const char* title);
    virtual ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

protected:
    virtual void onUpdate(float timeSeconds) = 0;
    virtual void onRender(const glm::mat4& projection, const glm::mat4& view) = 0;

protected:
    std::unique_ptr<Window> window_;
};



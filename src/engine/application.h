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

    void Run();

protected:
    virtual void OnUpdate(float time_seconds) = 0;
    virtual void OnRender() = 0;

protected:
    std::unique_ptr<Window> window_;
};



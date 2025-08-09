#include "application.h"
#include "window.h"

#include <GLFW/glfw3.h>

Application::Application(int width, int height, const char* title)
    : window_(new Window(width, height, title))
{
}

Application::~Application() = default;

void Application::Run()
{
    while (!window_->ShouldClose())
    {
        float t = static_cast<float>(glfwGetTime());
        OnUpdate(t);

        OnRender();

        window_->SwapBuffers();
        window_->PollEvents();
    }
}



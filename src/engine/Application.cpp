#include "application.h"
#include "window.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

Application::Application(int width, int height, const char* title)
    : window_(new Window(width, height, title))
{
}

Application::~Application() = default;

void Application::Run()
{
    const float aspect = static_cast<float>(window_->Width()) / static_cast<float>(window_->Height());
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    while (!window_->ShouldClose())
    {
        float t = static_cast<float>(glfwGetTime());
        OnUpdate(t);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        OnRender(projection, view);

        window_->SwapBuffers();
        window_->PollEvents();
    }
}



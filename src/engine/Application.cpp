#include "Application.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

Application::Application(int width, int height, const char* title)
    : window_(new Window(width, height, title))
{
}

Application::~Application() = default;

void Application::run()
{
    // Basic fixed camera for now
    const float aspect = static_cast<float>(window_->width()) / static_cast<float>(window_->height());
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    while (!window_->shouldClose())
    {
        float t = static_cast<float>(glfwGetTime());
        onUpdate(t);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        onRender(projection, view);

        window_->swapBuffers();
        window_->pollEvents();
    }
}



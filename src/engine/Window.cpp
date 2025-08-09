#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <stdexcept>

Window::Window(int width, int height, const char* title)
    : width_(width), height_(height)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindow_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!glfwWindow_)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(glfwWindow_);
    glfwSetFramebufferSizeCallback(glfwWindow_, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(glfwWindow_);
        glfwWindow_ = nullptr;
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width_, height_);
}

Window::~Window()
{
    if (glfwWindow_)
    {
        glfwDestroyWindow(glfwWindow_);
        glfwWindow_ = nullptr;
    }
    glfwTerminate();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(glfwWindow_);
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(glfwWindow_);
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

void Window::framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}



#include "window.h"

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

    glfw_window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!glfw_window_)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(glfw_window_);
    // Store pointer to this Window for callbacks
    glfwSetWindowUserPointer(glfw_window_, this);
    // Enable V-Sync by default to cap FPS and avoid CPU spin; can be made configurable
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(glfw_window_, FramebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(glfw_window_);
        glfw_window_ = nullptr;
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width_, height_);
}

Window::~Window()
{
    if (glfw_window_)
    {
        glfwDestroyWindow(glfw_window_);
        glfw_window_ = nullptr;
    }
    glfwTerminate();
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(glfw_window_);
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(glfw_window_);
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Update the viewport and cache new size on the Window instance
    if (window)
    {
        if (auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window)))
        {
            self->width_ = width;
            self->height_ = height;
        }
    }
    glViewport(0, 0, width, height);
}



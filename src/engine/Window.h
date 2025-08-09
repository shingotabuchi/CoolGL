#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
    Window(int width, int height, const char* title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void swapBuffers() const;
    void pollEvents() const;

    GLFWwindow* handle() const { return glfwWindow_; }

    int width() const { return width_; }
    int height() const { return height_; }

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* glfwWindow_ = nullptr;
    int width_ = 0;
    int height_ = 0;
};



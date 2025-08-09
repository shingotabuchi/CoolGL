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

    bool ShouldClose() const;
    void SwapBuffers() const;
    void PollEvents() const;

    GLFWwindow* Handle() const { return glfw_window_; }

    int Width() const { return width_; }
    int Height() const { return height_; }

private:
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* glfw_window_ = nullptr;
    int width_ = 0;
    int height_ = 0;
};



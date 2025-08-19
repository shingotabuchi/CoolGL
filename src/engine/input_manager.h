#pragma once

#include "i_input_listener.h"
#include <vector>
#include <GLFW/glfw3.h>

class InputManager
{
public:
    // Singleton access
    static InputManager &GetInstance()
    {
        static InputManager instance;
        return instance;
    }

    // Call this once after the window is created
    void Initialize(GLFWwindow *window);

    // Methods for components to register/unregister
    void AddListener(IInputListener *listener);
    void RemoveListener(IInputListener *listener);

private:
    // Private constructor for singleton pattern
    InputManager() = default;
    ~InputManager() = default;

    // Prohibit copying and assignment
    InputManager(const InputManager &) = delete;
    InputManager &operator=(const InputManager &) = delete;

    // Static callbacks that GLFW will call
    static void GlfwScrollCallback(GLFWwindow *window, double x_offset, double y_offset);

    // Member variables
    GLFWwindow *window_ = nullptr;
    std::vector<IInputListener *> listeners_;
};
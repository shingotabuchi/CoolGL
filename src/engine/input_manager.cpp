#include "input_manager.h"
#include <algorithm>

void InputManager::Initialize(GLFWwindow *window)
{
    window_ = window;

    // Set the user pointer to our singleton instance
    glfwSetWindowUserPointer(window_, this);

    // Set the static C-style functions as the callbacks
    glfwSetScrollCallback(window_, GlfwScrollCallback);
    // ... set other callbacks here (key, mouse button, etc.) ...
}

void InputManager::AddListener(IInputListener *listener)
{
    if (listener)
    {
        listeners_.push_back(listener);
    }
}

void InputManager::RemoveListener(IInputListener *listener)
{
    // Find and remove the listener from the vector
    listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
}

// The static callback implementation
void InputManager::GlfwScrollCallback(GLFWwindow *window, double x_offset, double y_offset)
{
    // Retrieve the InputManager instance
    auto *manager = static_cast<InputManager *>(glfwGetWindowUserPointer(window));
    if (manager)
    {
        // Forward the event to all registered listeners
        for (IInputListener *listener : manager->listeners_)
        {
            listener->OnScroll(y_offset);
        }
    }
}
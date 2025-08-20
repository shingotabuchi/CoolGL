#include "debug_camera_controller.h"
#include "../game_object.h"
#include "../transform.h"
#include "../scene.h"
#include "../input_manager.h" // Include the manager
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h> // Need the full header here
#include <iostream>

void DebugCameraController::OnStart()
{
    cached_transform_ = Owner()->GetComponent<Transform>();
    window_ = Owner()->GetScene()->GetWindow();

    // Register this component with the InputManager
    InputManager::GetInstance().AddListener(this);
}

// void DebugCameraController::OnDestroy()
// {
//     // Unregister when the component is destroyed to prevent dangling pointers
//     InputManager::GetInstance().RemoveListener(this);
// }

// The scroll logic is now in its own dedicated handler
void DebugCameraController::OnScroll(double y_offset)
{
    scroll_y_ += y_offset;
}

void DebugCameraController::OnUpdate(float timeSeconds)
{
    if (!cached_transform_ || !window_)
        return;

    // ... (The mouse polling logic for rotation and panning remains the same) ...
    // Mouse position and delta
    double mouse_x = 0.0, mouse_y = 0.0;
    glfwGetCursorPos(window_, &mouse_x, &mouse_y);
    if (first_mouse_sample_)
    {
        last_mouse_x_ = mouse_x;
        last_mouse_y_ = mouse_y;
        first_mouse_sample_ = false;
    }
    const double dx = mouse_x - last_mouse_x_;
    const double dy = mouse_y - last_mouse_y_;
    last_mouse_x_ = mouse_x;
    last_mouse_y_ = mouse_y;

    const bool right_down = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    const bool left_down = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Camera Rotation
    if (right_down)
    {
        cached_transform_->rotation_euler.y += static_cast<float>(dx) * rotate_sensitivity;
        cached_transform_->rotation_euler.x += static_cast<float>(dy) * rotate_sensitivity;
        cached_transform_->rotation_euler.x = glm::clamp(cached_transform_->rotation_euler.x, -89.0f, 89.0f);
    }

    // Panning
    glm::quat current_orientation = glm::quat(glm::radians(cached_transform_->rotation_euler));
    if (left_down)
    {
        const glm::mat3 rot = glm::mat3_cast(current_orientation);
        const glm::vec3 cam_right = glm::normalize(rot * glm::vec3(1, 0, 0));
        const glm::vec3 cam_up = glm::normalize(rot * glm::vec3(0, 1, 0));
        const glm::vec3 pan_delta = cam_right * static_cast<float>(-dx) * pan_sensitivity + cam_up * static_cast<float>(dy) * pan_sensitivity;
        cached_transform_->position += pan_delta;
    }

    // Zooming (using the value from OnScroll)
    if (scroll_y_ != 0.0)
    {
        const glm::mat3 rot = glm::mat3_cast(current_orientation);
        const glm::vec3 cam_forward = glm::normalize(rot * glm::vec3(0, 0, -1));
        cached_transform_->position += cam_forward * static_cast<float>(scroll_y_) * zoom_speed;
        scroll_y_ = 0.0;
    }
}

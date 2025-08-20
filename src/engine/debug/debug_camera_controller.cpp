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

    const bool right_arrow_down = glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS;
    const bool left_arrow_down = glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS;
    const bool up_arrow_down = glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS;
    const bool down_arrow_down = glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS;

    const bool w_down = glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS;
    const bool a_down = glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS;
    const bool s_down = glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS;
    const bool d_down = glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS;

    double arrow_dx = 0;
    double arrow_dy = 0;
    if (right_arrow_down)
    {
        arrow_dx -= 0.1;
    }
    if (left_arrow_down)
    {
        arrow_dx += 0.1;
    }
    if (up_arrow_down)
    {
        arrow_dy += 0.1;
    }
    if (down_arrow_down)
    {
        arrow_dy -= 0.1;
    }

    double wasd_forward = 0;
    double wasd_right = 0;

    if (w_down)
    {
        wasd_forward += 0.1;
    }
    if (s_down)
    {
        wasd_forward -= 0.1;
    }
    if (a_down)
    {
        wasd_right -= 0.1;
    }
    if (d_down)
    {
        wasd_right += 0.1;
    }

    // Camera Rotation
    if (right_down || right_arrow_down || left_arrow_down || up_arrow_down || down_arrow_down)
    {
        cached_transform_->rotation_euler.y += static_cast<float>(dx + arrow_dx) * rotate_sensitivity;
        cached_transform_->rotation_euler.x += static_cast<float>(dy + arrow_dy) * rotate_sensitivity;
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
    if (wasd_forward != 0.0 || wasd_right != 0.0)
    {
        const glm::mat3 rot = glm::mat3_cast(current_orientation);
        const glm::vec3 cam_right = glm::normalize(rot * glm::vec3(1, 0, 0));
        // const glm::vec3 forward = glm::cross(cam_right, glm::vec3(0, 1, 0));
        const glm::vec3 forward = glm::normalize(rot * glm::vec3(0, 0, -1));
        cached_transform_->position += forward * static_cast<float>(wasd_forward) * pan_sensitivity * 2.0f;
        cached_transform_->position += cam_right * static_cast<float>(wasd_right) * pan_sensitivity * 2.0f;
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

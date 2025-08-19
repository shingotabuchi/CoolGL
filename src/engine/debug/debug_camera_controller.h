#pragma once

#include "../component.h"
#include "../i_input_listener.h"

struct GLFWwindow;
class Transform;

class DebugCameraController : public Component, public IInputListener
{
public:
    float rotate_sensitivity = 0.2f;
    float pan_sensitivity = 0.01f;
    float zoom_speed = 0.5f;

    void OnStart() override;
    void OnUpdate(float timeSeconds) override;

    void OnScroll(double y_offset) override;

private:
    mutable Transform *cached_transform_ = nullptr;
    GLFWwindow *window_ = nullptr;

    bool first_mouse_sample_ = true;
    double last_mouse_x_ = 0.0;
    double last_mouse_y_ = 0.0;
    double scroll_y_ = 0.0;
};

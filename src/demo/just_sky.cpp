#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>
#include "engine/texture_loader.h"
#include "engine/texture.h"
#include "engine/application.h"
#include "engine/window.h"
#include "engine/renderer.h"
#include "engine/model_loader.h"
#include "engine/shader.h"
#include "engine/mesh_creator.h"
#include "engine/scene.h"
#include "engine/game_object.h"
#include "engine/transform.h"
#include "engine/mesh_renderer.h"
#include "engine/camera.h"
#include "engine/light.h"
#include "engine/input_manager.h"
#include "engine/debug/debug_camera_controller.h"
#include <assimp/postprocess.h>
#include <string>
#include <iostream>

class JustSkyApp : public Application
{
public:
    JustSkyApp() : Application(800, 800, "Cool GL")
    {
        GLFWwindow *win = window_->Handle();
        InputManager::GetInstance().Initialize(win);
        scene_.SetWindow(win);

        // Create camera object (must exist to render)
        GameObject &camObj = scene_.CreateObject();
        auto *camTransform = camObj.AddComponent<Transform>();
        camTransform->position = glm::vec3(0.0f, 4.0f, -8.0f);
        camTransform->rotation_euler = glm::vec3(-7.0f, 180.0f, 0.0f);
        auto *camera = camObj.AddComponent<Camera>();
        camera->field_of_view_degrees = 60.0f;
        camera_ = camera;

        camObj.AddComponent<DebugCameraController>();

        // Initialize camera control state
        camera_transform_ = camTransform;

        // Create a directional light object
        GameObject &lightObj = scene_.CreateObject();
        auto *lightTransform = lightObj.AddComponent<Transform>();
        lightTransform->position = glm::vec3(0.0f, 3.0f, 0.0f);
        lightTransform->rotation_euler = glm::vec3(45.0f, -120.0f, 0.0f);
        auto *light = lightObj.AddComponent<Light>();
        light->color = glm::vec3(1.0f, 0.9568627f, 0.8392157f);
        light->intensity = 1.0f;

        // Configure scene sky and ambient from equirectangular texture
        scene_.SetSkyFromEquirect("resources/cat/catsky.png");
    }

protected:
    void OnUpdate(float timeSeconds) override
    {
        scene_.Update(timeSeconds);
    }

    void OnRender() override
    {
        static Renderer renderer;
        scene_.Render(renderer);
    }

private:
    Scene scene_{};
    Transform *cat_transform_ = nullptr;
    float catRotationSpeed_ = 1.0f;
    Camera *camera_ = nullptr;
    Transform *camera_transform_ = nullptr;

    // Mouse state
    bool first_mouse_sample_ = true;
    double last_mouse_x_ = 0.0;
    double last_mouse_y_ = 0.0;
};

int main()
{
    JustSkyApp app;
    app.Run();
    return 0;
}

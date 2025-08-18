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
#include <assimp/postprocess.h>
#include <string>

// Accumulate mouse wheel scroll between frames
static double g_scroll_y_ = 0.0;

class ExperimentApp : public Application
{
public:
    ExperimentApp() : Application(800, 800, "Cool GL")
    {
        // Build scene

        // Create cat object
        GameObject& cat = scene_.CreateObject();
        auto* transform = cat.AddComponent<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);
        transform->rotation_euler = glm::vec3(-90.0f, 0.0f, 0.0f);
        cat_transform_ = transform;
        Mesh mesh = ModelLoader::LoadFirstMeshFromFile("resources/cat/cat.fbx");
        auto meshPtrCat = std::make_shared<Mesh>(std::move(mesh));
        auto catMat = std::make_shared<Material>();
        catMat->vertex_shader_path = "src/engine/shaders/lit.vert";
        catMat->fragment_shader_path = "src/engine/shaders/lit.frag";
        catMat->albedo_texture_path = "resources/cat/cattex.png";
        catMat->color = glm::vec3(1.0f, 1.0f, 1.0f);
        catMat->smoothness = 0.6f;
        auto* renderer = cat.AddComponent<MeshRenderer>(meshPtrCat, catMat);

        // Create cat clone
        GameObject& catClone = scene_.Instantiate(cat);
        auto* cloneTransform = catClone.GetComponent<Transform>();
        cloneTransform->position = glm::vec3(1.0f, 0.0f, -2.0f);
        cloneTransform->rotation_euler = glm::vec3(90.0f, 180.0f, 0.0f);

        // Create station 
        std::vector<Mesh> stationMeshes = ModelLoader::LoadAllMeshesFromFile("resources/station/station.fbx", true);
        auto stationMat = std::make_shared<Material>();
        stationMat->vertex_shader_path = "src/engine/shaders/lit.vert";
        stationMat->fragment_shader_path = "src/engine/shaders/lit.frag";
        stationMat->albedo_texture_path = "resources/station/station.png";
        catMat->color = glm::vec3(1.0f, 1.0f, 1.0f);
        stationMat->smoothness = 0.5f;
        for (auto& m : stationMeshes)
        {
            GameObject& station_part = scene_.CreateObject();
            auto* transform = station_part.AddComponent<Transform>();
            transform->position = glm::vec3(-1.625f, 0.0f, -3.5f);
            transform->rotation_euler = glm::vec3(0.0f, -90.0f, 0.0f);
            transform->scale *= 0.0175f;
            auto meshPtr = std::make_shared<Mesh>(std::move(m));
            auto* mr = station_part.AddComponent<MeshRenderer>(meshPtr, stationMat);
        }

        // Create camera object (must exist to render)
        GameObject& camObj = scene_.CreateObject();
        auto* camTransform = camObj.AddComponent<Transform>();
        camTransform->position = glm::vec3(0.0f, 4.0f, -8.0f);
        camTransform->rotation_euler = glm::vec3(7.0f, 180.0f, 0.0f);
        // camTransform->position = glm::vec3(0.0f, 1.49f, 5.0f);
        // camTransform->rotation_euler = glm::vec3(0.0f, 0.0f, 0.0f);
        auto* camera = camObj.AddComponent<Camera>();
        // By default, camera derives aspect from the current window/viewport
        // Set camera->sync_aspect_with_window = false to control aspect manually
        camera->field_of_view_degrees = 60.0f;
        // camera->projection_type = Camera::ProjectionType::Orthographic;
        // camera->orthographic_size = 5.0f;
        camera_ = camera;

        // Initialize camera control state
        camera_transform_ = camTransform;
        cam_orientation_ = glm::quat(glm::radians(camera_transform_->rotation_euler));

        // Register a scroll callback for zooming
        glfwSetScrollCallback(window_->Handle(), [](GLFWwindow* /*w*/, double /*xoff*/, double yoff) {
            g_scroll_y_ += yoff;
        });

        // Create a directional light object
        GameObject& lightObj = scene_.CreateObject();
        auto* lightTransform = lightObj.AddComponent<Transform>();
        lightTransform->position = glm::vec3(0.0f, 3.0f, 0.0f);
        lightTransform->rotation_euler = glm::vec3(-90.0f, -150.0f, 0.0f);
        auto* light = lightObj.AddComponent<Light>();
        light->color = glm::vec3(1.0f, 0.9568627f, 0.8392157f); 
        light->intensity = 1.0f;

        // Configure scene sky and ambient from equirectangular texture
        scene_.SetSkyFromEquirect("resources/cat/catsky.png");
    }

protected:
    void OnUpdate(float timeSeconds) override
    {
        scene_.Update(timeSeconds);

        if (!camera_transform_) return;

        GLFWwindow* win = window_->Handle();

        // Mouse position and delta
        double mouse_x = 0.0, mouse_y = 0.0;
        glfwGetCursorPos(win, &mouse_x, &mouse_y);
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

        const bool right_down = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        const bool middle_down = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

        // Tunables
        const float rotate_sensitivity = 0.2f; // degrees per pixel
        const float pan_sensitivity = 0.01f;   // world units per pixel
        const float zoom_speed = 0.5f;         // world units per scroll step

        // Right mouse: rotate camera around CAM up (horizontal) and CAM right (vertical)
        if (right_down)
        {
            const float yaw_deg   = static_cast<float>(dx) * rotate_sensitivity;
            const float pitch_deg = static_cast<float>(-dy) * rotate_sensitivity;

            // Build basis from current orientation
            const glm::mat3 rot = glm::mat3_cast(cam_orientation_);
            const glm::vec3 cam_right = glm::normalize(rot * glm::vec3(1, 0, 0));
            const glm::vec3 cam_up    = glm::normalize(rot * glm::vec3(0, 1, 0));

            // Apply yaw about camera up
            if (yaw_deg != 0.0f)
            {
                const glm::quat q_yaw = glm::angleAxis(glm::radians(yaw_deg), cam_up);
                cam_orientation_ = glm::normalize(q_yaw * cam_orientation_);
            }

            // Recompute right after yaw for consistent pitch axis
            {
                const glm::mat3 rot2 = glm::mat3_cast(cam_orientation_);
                const glm::vec3 right2 = glm::normalize(rot2 * glm::vec3(1, 0, 0));
                if (pitch_deg != 0.0f)
                {
                    const glm::quat q_pitch = glm::angleAxis(glm::radians(pitch_deg), right2);
                    cam_orientation_ = glm::normalize(q_pitch * cam_orientation_);
                }
            }

            // Sync back to Transform
            const glm::vec3 euler_xyz = glm::degrees(glm::eulerAngles(cam_orientation_));
            camera_transform_->rotation_euler = euler_xyz;
        }

        // Middle mouse: pan along camera right/up. Match mouse direction (no axis invert)
        if (middle_down)
        {
            const glm::mat3 rot = glm::mat3_cast(cam_orientation_);
            const glm::vec3 cam_right = glm::normalize(rot * glm::vec3(1, 0, 0));
            const glm::vec3 cam_up    = glm::normalize(rot * glm::vec3(0, 1, 0));
            const glm::vec3 pan_delta = cam_right * static_cast<float>(dx) * pan_sensitivity
                                      + cam_up    * static_cast<float>(-dy) * pan_sensitivity; // -dy so drag up moves up
            camera_transform_->position += pan_delta;
        }

        // Scroll: dolly zoom along camera forward
        if (g_scroll_y_ != 0.0)
        {
            const glm::mat3 rot = glm::mat3_cast(cam_orientation_);
            const glm::vec3 cam_forward = glm::normalize(rot * glm::vec3(0, 0, -1));
            camera_transform_->position += cam_forward * static_cast<float>(g_scroll_y_) * zoom_speed;
            g_scroll_y_ = 0.0;
        }
    }

    void OnRender() override
    {
        static Renderer renderer;
        scene_.Render(renderer);
    }

private:
    Scene scene_{};
    Transform* cat_transform_ = nullptr;
    float catRotationSpeed_ = 1.0f;
    Camera* camera_ = nullptr;
    Transform* camera_transform_ = nullptr;

    // Orientation state (quaternion) for camera-space rotations
    glm::quat cam_orientation_{};

    // Mouse state
    bool first_mouse_sample_ = true;
    double last_mouse_x_ = 0.0;
    double last_mouse_y_ = 0.0;
};

int main()
{
    ExperimentApp app;
    app.Run();
    return 0;
}

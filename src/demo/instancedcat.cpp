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

class ExperimentApp : public Application
{
public:
    ExperimentApp() : Application(800, 800, "Cool GL")
    {
        GLFWwindow *win = window_->Handle();
        InputManager::GetInstance().Initialize(win);
        scene_.SetWindow(win);

        // Create cat object
        GameObject &cat = scene_.CreateObject();
        auto *catTransform_ = cat.AddComponent<Transform>();
        catTransform_->position = glm::vec3(0.0f, 0.0f, 0.0f);
        catTransform_->rotation_euler = glm::vec3(-90.0f, 180.0f, 0.0f);
        Mesh mesh = ModelLoader::LoadFirstMeshFromFile("resources/cat/cat.fbx");
        auto meshPtrCat = std::make_shared<Mesh>(std::move(mesh));
        meshPtrCat->instance_id = 1;
        std::vector<glm::mat4> cat_matrices;
        cat_matrices.reserve(10000);
        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 100; j++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(i - 50, 0.0f, j));
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

                cat_matrices.push_back(model);
            }
        }
        meshPtrCat->CreateInstanceBuffer(cat_matrices);
        auto catMat = std::make_shared<Material>();
        catMat->vertex_shader_path = "src/engine/shaders/lit.vert";
        catMat->fragment_shader_path = "src/engine/shaders/lit.frag";
        catMat->albedo_texture_path = "resources/cat/cattex.png";
        catMat->color = glm::vec3(1.0f, 1.0f, 1.0f);
        catMat->smoothness = 0.6f;
        auto *cat_renderer = cat.AddComponent<MeshRenderer>(meshPtrCat, catMat);

        // Create plane object
        GameObject &plane = scene_.CreateObject();
        auto *plane_transform = plane.AddComponent<Transform>();
        plane_transform->position = glm::vec3(0.0f, 0.0f, 0.0f);
        plane_transform->rotation_euler = glm::vec3(0.0f, 0.0f, 0.0f);
        plane_transform->scale = glm::vec3(100.0f, 100.0f, 100.0f);
        Mesh plane_mesh = MeshCreator::CreateUnitPlane();
        auto plane_mesh_ptr = std::make_shared<Mesh>(std::move(plane_mesh));
        auto plane_mat = std::make_shared<Material>();
        plane_mat->vertex_shader_path = "src/engine/shaders/lit.vert";
        plane_mat->fragment_shader_path = "src/engine/shaders/lit.frag";
        plane_mat->color = glm::vec3(1.0f, 1.0f, 1.0f);
        plane_mat->smoothness = 0.6f;
        plane.AddComponent<MeshRenderer>(plane_mesh_ptr, plane_mat);

        // Create camera object (must exist to render)
        GameObject &cam_obj = scene_.CreateObject();
        auto *cam_transform = cam_obj.AddComponent<Transform>();
        cam_transform->position = glm::vec3(0.0f, 30.0f, -30.0f);
        cam_transform->rotation_euler = glm::vec3(-45.0f, 180.0f, 0.0f);
        auto *camera = cam_obj.AddComponent<Camera>();
        camera->field_of_view_degrees = 60.0f;

        cam_obj.AddComponent<DebugCameraController>();

        // Create a directional light object
        GameObject &light_obj = scene_.CreateObject();
        auto *light_transform = light_obj.AddComponent<Transform>();
        light_transform->position = glm::vec3(0.0f, 3.0f, 0.0f);
        light_transform->rotation_euler = glm::vec3(-45.0f, 60.0f, 0.0f);
        auto *light = light_obj.AddComponent<Light>();
        light->color = glm::vec3(1.0f, 0.9568627f, 0.8392157f);
        light->intensity = 1.0f;

        // Configure scene sky and ambient from equirectangular texture
        scene_.SetSkyFromEquirect("resources/cat/catsky.png");
    }

protected:
    void OnUpdate(float time_seconds) override
    {
        frame_count_++;
        float diff_time = time_seconds - last_time_;
        if (frame_count_ > fps_calc_interval_)
        {
            const float fps = frame_count_ / diff_time;
            last_time_ = time_seconds;
            frame_count_ = 0;
            if (!first_fps_)
            {
                max_fps_ = std::max(max_fps_, fps);
                min_fps_ = std::min(min_fps_, fps);
                std::cout << min_fps_ << " " << max_fps_ << " " << fps << std::endl;
            }
            else
            {
                first_fps_ = false;
            }
        }
        scene_.Update(time_seconds);
    }

    void OnRender() override
    {
        static Renderer renderer;
        scene_.Render(renderer);
    }

private:
    Scene scene_{};
    float catRotationSpeed_ = 1.0f;

    // Mouse state
    bool first_mouse_sample_ = true;
    bool first_fps_ = true;
    double last_mouse_x_ = 0.0;
    double last_mouse_y_ = 0.0;
    float last_time_;
    float max_fps_ = -1.0f;
    float min_fps_ = 1e10;
    int fps_calc_interval_ = 100;
    int frame_count_ = 0;
};

int main()
{
    ExperimentApp app;
    app.Run();
    return 0;
}

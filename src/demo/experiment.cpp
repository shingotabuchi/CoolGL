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
        catTransform_->rotation_euler = glm::vec3(-90.0f, 0.0f, 0.0f);
        Mesh mesh = ModelLoader::LoadFirstMeshFromFile("resources/cat/cat.fbx");
        auto meshPtrCat = std::make_shared<Mesh>(std::move(mesh));
        auto catMat = std::make_shared<Material>();
        catMat->vertex_shader_path = "src/engine/shaders/lit.vert";
        catMat->fragment_shader_path = "src/engine/shaders/lit.frag";
        catMat->albedo_texture_path = "resources/cat/cattex.png";
        catMat->color = glm::vec3(1.0f, 1.0f, 1.0f);
        catMat->smoothness = 0.6f;
        cat.AddComponent<MeshRenderer>(meshPtrCat, catMat);

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
        cam_transform->position = glm::vec3(0.0f, 4.0f, -8.0f);
        cam_transform->rotation_euler = glm::vec3(-7.0f, 180.0f, 0.0f);
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

        // // Light clone
        // GameObject &light_clone_obj = scene_.Instantiate(light_obj);
        // auto *light_clone_transform = light_clone_obj.GetComponent<Transform>();
        // light_clone_transform->rotation_euler = glm::vec3(45.0f, 60.0f, 0.0f);

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
    float catRotationSpeed_ = 1.0f;

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

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/texture_loader.h"
#include "engine/texture.h"
#include "engine/application.h"
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

        // Create station 
        std::vector<Mesh> stationMeshes = ModelLoader::LoadAllMeshesFromFile("resources/station/station.fbx", true);
        auto stationMat = std::make_shared<Material>();
        stationMat->vertex_shader_path = "src/engine/shaders/lit.vert";
        stationMat->fragment_shader_path = "src/engine/shaders/lit.frag";
        stationMat->color = glm::vec3(0.5f, 0.5f, 0.5f);
        stationMat->smoothness = 0.4f;
        for (auto& m : stationMeshes)
        {
            GameObject& station_part = scene_.CreateObject();
            auto* transform = station_part.AddComponent<Transform>();
            // transform->position = glm::vec3(0.0f, 0.0f, 0.0f);
            transform->rotation_euler = glm::vec3(0.0f, -90.0f, 0.0f);
            transform->scale *= 0.01f;
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

        // Create a directional light object
        GameObject& lightObj = scene_.CreateObject();
        auto* lightTransform = lightObj.AddComponent<Transform>();
        lightTransform->position = glm::vec3(0.0f, 3.0f, 0.0f);
        lightTransform->rotation_euler = glm::vec3(-50.0f, -150.0f, 0.0f);
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
};

int main()
{
    ExperimentApp app;
    app.Run();
    return 0;
}

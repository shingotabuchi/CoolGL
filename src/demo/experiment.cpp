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
#include <string>

static const char *vertexShaderSrc = R"glsl(
    #version 410 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aUV;
    uniform mat4 uMVP;
    out vec3 vNormal;
    out vec2 vUV;
    void main() {
        gl_Position = uMVP * vec4(aPos, 1.0);
        vNormal = aNormal;
        vUV = aUV;
    }
)glsl";

static const char *fragmentShaderSrc = R"glsl(
    #version 410 core
    const int MAX_LIGHTS = 4;
    out vec4 FragColor;
    in vec3 vNormal;
    in vec2 vUV;
    uniform int uLightCount;
    uniform vec3 uLightDirs[MAX_LIGHTS];
    uniform vec3 uLightColors[MAX_LIGHTS];
    uniform vec3 uAmbient; // ambient color derived from skybox
    uniform sampler2D uAlbedo;
    uniform int uUseTexture;
    void main() {
        vec3 N = normalize(vNormal);
        vec3 accum = uAmbient;
        for (int i = 0; i < uLightCount; ++i) {
            vec3 L = normalize(uLightDirs[i]);
            float diff = max(dot(N, L), 0.0);
            accum += uLightColors[i] * diff;
        }
        vec3 baseColor = (uUseTexture != 0) ? texture(uAlbedo, vUV).rgb : vec3(1.0);
        FragColor = vec4(baseColor * accum, 1.0);
    }
)glsl";

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
        Shader shader(vertexShaderSrc, fragmentShaderSrc);
        auto* renderer = cat.AddComponent<MeshRenderer>(std::move(mesh), std::move(shader));
        auto tex = std::make_shared<Texture>();
        TextureLoader::LoadTexture2DFromFile("resources/cat/cattex.png", false, *tex);
        renderer->diffuse_texture = std::move(tex);

        // Create cat clone
        GameObject& catClone = scene_.Instantiate(cat);
        auto* cloneTransform = catClone.GetComponent<Transform>();
        cloneTransform->position = glm::vec3(1.0f, 0.0f, -2.0f);

        // Create station
        GameObject& station = scene_.CreateObject();
        auto* stationTransform = station.AddComponent<Transform>();
        stationTransform->position = glm::vec3(-1.827f, 0.0f, 2.6f);
        stationTransform->rotation_euler = glm::vec3(0.0f, 0.0f, 0.0f);
        stationTransform->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 2.0f;
        Mesh stationMesh = ModelLoader::LoadFirstMeshFromFile("resources/station/station.fbx");
        Shader stationShader(vertexShaderSrc, fragmentShaderSrc);
        auto* stationRenderer = station.AddComponent<MeshRenderer>(std::move(stationMesh), std::move(stationShader));

        // Create camera object (must exist to render)
        GameObject& camObj = scene_.CreateObject();
        auto* camTransform = camObj.AddComponent<Transform>();
        camTransform->position = glm::vec3(0.0f, 4.0f, -8.0f);
        camTransform->rotation_euler = glm::vec3(7.0f, 180.0f, 0.0f);
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

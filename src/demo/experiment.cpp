#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/application.h"
#include "engine/renderer.h"
#include "engine/model_loader.h"
#include "engine/shader.h"
#include "engine/scene.h"
#include "engine/game_object.h"
#include "engine/transform.h"
#include "engine/mesh_renderer.h"
#include "engine/camera.h"

static const char *vertexShaderSrc = R"glsl(
    #version 410 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    uniform mat4 uMVP;
    out vec3 vPos;
    out vec3 vNormal;
    void main() {
        gl_Position = uMVP * vec4(aPos, 1.0);
        vPos = aPos;
        vNormal = aNormal;
    }
)glsl";

static const char *fragmentShaderSrc = R"glsl(
    #version 410 core
    out vec4 FragColor;
    in vec3 vPos;
    in vec3 vNormal;
    uniform vec3 uLightPos;
    uniform vec3 uLightColor;
    void main() {
        vec3 lightDir = normalize(uLightPos - vPos);
        float diff = max(dot(vNormal, lightDir), 0.0);
        vec3 diffuse = uLightColor * diff;
        FragColor = vec4(diffuse, 1.0);
    }
)glsl";

class ExperimentApp : public Application
{
public:
    ExperimentApp() : Application(800, 800, "Cool GL")
    {
        // Build scene
        GameObject& cat = scene_.CreateObject();
        auto* transform = cat.AddComponent<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);
        transform->rotation_euler = glm::vec3(-90.0f, 0.0f, 0.0f);
        cat_transform_ = transform;

        Mesh mesh = ModelLoader::LoadFirstMeshFromFile("resources/cat/cat.fbx");
        Shader shader(vertexShaderSrc, fragmentShaderSrc);
        auto* renderer = cat.AddComponent<MeshRenderer>(std::move(mesh), std::move(shader));
        renderer->light_pos = glm::vec3(0.0f, 0.0f, 10000.0f);
        renderer->light_color = glm::vec3(1.0f, 1.0f, 1.0f);

        // Create camera object (must exist to render)
        GameObject& camObj = scene_.CreateObject();
        auto* camTransform = camObj.AddComponent<Transform>();
        camTransform->position = glm::vec3(0.0f, 1.49f, 3.26f);
        // camTransform->position = glm::vec3(0.0f, 0.0f, 3.26f);
        auto* camera = camObj.AddComponent<Camera>();
        camera->aspect_ratio = 800.0f / 800.0f; // initial aspect; could be updated on resize
        camera->field_of_view_degrees = 60.0f;
        camera_ = camera;
    }

protected:
    void OnUpdate(float timeSeconds) override
    {
        scene_.Update(timeSeconds);
    }

    void OnRender() override
    {
        static Renderer renderer;
        renderer.BeginFrame(0.1f, 0.2f, 0.3f, 1.0f);
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

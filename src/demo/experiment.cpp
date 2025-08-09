#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/Application.h"
#include "engine/Renderer.h"
#include "engine/ModelLoader.h"
#include "engine/Shader.h"
#include "engine/Scene.h"
#include "engine/GameObject.h"
#include "engine/Transform.h"
#include "engine/MeshRenderer.h"
#include "engine/Rotator.h"

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
        GameObject& cat = scene_.createObject();
        auto* transform = cat.addComponent<Transform>();
        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

        Mesh mesh = ModelLoader::loadFirstMeshFromFile("resources/cat/cat.fbx");
        Shader shader(vertexShaderSrc, fragmentShaderSrc);
        auto* renderer = cat.addComponent<MeshRenderer>(std::move(mesh), std::move(shader));
        renderer->lightPos = glm::vec3(0.0f, 0.0f, 10000.0f);
        renderer->lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

        cat.addComponent<Rotator>()->speedRadiansPerSec = 1.0f;
    }

protected:
    void onUpdate(float timeSeconds) override
    {
        scene_.update(timeSeconds);
    }

    void onRender(const glm::mat4& projection, const glm::mat4& view) override
    {
        static Renderer renderer;
        renderer.beginFrame(0.1f, 0.2f, 0.3f, 1.0f);
        scene_.render(renderer, projection, view);
    }

private:
    Scene scene_{};
};

int main()
{
    ExperimentApp app;
    app.run();
    return 0;
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/Application.h"
#include "engine/Renderer.h"
#include "engine/ModelLoader.h"
#include "engine/Shader.h"

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
        mesh_ = ModelLoader::loadFirstMeshFromFile("resources/cat/cat.fbx");
        shader_ = Shader(vertexShaderSrc, fragmentShaderSrc);
        lightPos_ = glm::vec3(0.0f, 0.0f, 10000.0f);
        lightColor_ = glm::vec3(1.0f, 1.0f, 1.0f);
    }

protected:
    void onUpdate(float timeSeconds) override
    {
        model_ = glm::rotate(glm::mat4(1.0f), timeSeconds, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void onRender(const glm::mat4& projection, const glm::mat4& view) override
    {
        static Renderer renderer;
        renderer.beginFrame(0.1f, 0.2f, 0.3f, 1.0f);
        glm::mat4 mvp = projection * view * model_;
        renderer.drawMesh(mesh_, shader_, mvp, lightPos_, lightColor_);
    }

private:
    Mesh mesh_{};
    Shader shader_{};
    glm::mat4 model_{1.0f};
    glm::vec3 lightPos_{};
    glm::vec3 lightColor_{};
};

int main()
{
    ExperimentApp app;
    app.run();
    return 0;
}

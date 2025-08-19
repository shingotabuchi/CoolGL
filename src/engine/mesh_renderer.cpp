#include "mesh_renderer.h"
#include "renderer.h"
#include "mesh_creator.h"
#include "game_object.h"
#include "transform.h"
#include "scene.h"
#include "light.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Simple skybox shader: equirectangular 2D texture sampled by direction
static const char *kSkyVS = R"glsl(
    #version 410 core
    layout(location=0) in vec3 aPos;
    uniform mat4 uProj;
    uniform mat4 uViewNoT;
    out vec3 vDir;
    void main(){
        vec4 pos = uProj * uViewNoT * vec4(aPos,1.0);
        gl_Position = vec4(pos.xy, pos.w, pos.w);
        vDir = aPos;
    }
)glsl";

static const char *kSkyFS = R"glsl(
    #version 410 core
    in vec3 vDir;
    out vec4 FragColor;
    uniform sampler2D uSky;
    vec2 dirToEquirectUV(vec3 d){
        d = normalize(d);
        float phi = atan(d.z, d.x);
        float theta = asin(clamp(d.y, -1.0, 1.0));
        float u = 0.5 + phi / (2.0*3.14159265359);
        float v = 0.5 - theta / 3.14159265359;
        return vec2(u,v);
    }
    void main(){
        vec2 uv = dirToEquirectUV(vDir);
        vec3 c = texture(uSky, uv).rgb;
        FragColor = vec4(c, 1.0);
    }
)glsl";

static std::shared_ptr<Mesh> g_unitCube;
std::shared_ptr<Mesh> MeshRenderer::CreateUnitCube()
{
    if (g_unitCube)
        return g_unitCube;
    // Delegate geometry to MeshCreator so we have proper normals/UVs when needed
    g_unitCube = std::make_shared<Mesh>(MeshCreator::CreateUnitCube());
    return g_unitCube;
}

void MeshRenderer::OnRender(Renderer &renderer, const glm::mat4 &projection, const glm::mat4 &view)
{
    if (render_mode == RenderMode::Skybox)
    {
        const bool hasTexture = (diffuse_texture && diffuse_texture->is_valid());
        // Build rotation-only view matrix (no translation)
        glm::mat4 v = view;
        v[3][0] = 0.0f;
        v[3][1] = 0.0f;
        v[3][2] = 0.0f;

        // Depth state for background: disable depth test for guaranteed visibility
        GLboolean wasDepthEnabled = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        // Culling: disable for robustness (mesh winding might vary)
        GLboolean wasCullEnabled = glIsEnabled(GL_CULL_FACE);
        GLint oldCullFace = GL_BACK;
        if (wasCullEnabled)
            glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFace);
        glDisable(GL_CULL_FACE);

        // Lazy init a simple skybox shader if none provided
        if (!shader_ || shader_->id() == 0)
        {
            shader_ = std::make_shared<Shader>(kSkyVS, kSkyFS);
        }
        // Bind uniforms and texture
        shader_->use();
        // If the provided projection is orthographic, substitute a perspective-like
        // projection for the skybox so it fills the screen.
        glm::mat4 projForSky = projection;
        const float eps = 1e-6f;
        // GLM stores matrices in column-major; perspective has m[3][3] ~= 0, ortho has ~= 1
        if (glm::abs(projection[3][3] - 1.0f) < eps)
        {
            GLint vp[4] = {0, 0, 1, 1};
            glGetIntegerv(GL_VIEWPORT, vp);
            const float w = static_cast<float>(vp[2]);
            const float h = static_cast<float>(vp[3]);
            const float ar = (w > 0.0f && h > 0.0f) ? (w / h) : 1.0f;
            // Use a reasonable FOV to avoid distortion; skybox depth doesn't matter due to xyww trick
            projForSky = glm::perspective(glm::radians(60.0f), ar, 0.1f, 10.0f);
        }
        shader_->set_mat4("uProj", projForSky);
        shader_->set_mat4("uViewNoT", v);
        if (hasTexture)
        {
            diffuse_texture->bind(GL_TEXTURE_2D, 0);
            shader_->set_int("uSky", 0);
        }

        // Use unit cube mesh if none assigned
        std::shared_ptr<Mesh> box = mesh_ ? mesh_ : CreateUnitCube();
        box->Bind();
        box->Draw();

        // Restore depth/cull state
        if (wasCullEnabled)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(oldCullFace);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        if (wasDepthEnabled)
            glEnable(GL_DEPTH_TEST);
        return;
    }

    if (!cached_transform_ && Owner())
    {
        cached_transform_ = Owner()->GetComponent<Transform>();
    }
    const Transform *t = cached_transform_;
    glm::mat4 model = t ? t->LocalToWorld() : glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;

    // Precompute inverse(model) for transforming directions to object space
    const glm::mat4 invModel = glm::inverse(model);

    // Fetch lights from scene and transform their world directions into object space
    glm::vec3 lightDirs[Light::kMaxLights];
    glm::vec3 lightColors[Light::kMaxLights];
    int lightCount = 0;
    if (Owner() && Owner()->GetScene())
    {
        const auto &lights = Owner()->GetScene()->GetLights();
        for (size_t i = 0; i < lights.size() && lightCount < Light::kMaxLights; ++i)
        {
            const Light *light = lights[i];
            if (!light)
                continue;
            const glm::vec3 worldDir = glm::normalize(light->WorldDirection());
            glm::vec3 objectDir = glm::vec3(invModel * glm::vec4(worldDir, 0.0f));
            lightDirs[lightCount] = glm::normalize(objectDir);
            lightColors[lightCount] = light->color * light->intensity;
            ++lightCount;
        }
    }
    if (lightCount == 0)
    {
        // Fallback to a default light if none present
        lightDirs[0] = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
        lightColors[0] = light_color;
        lightCount = 1;
    }

    // Resolve shader and textures from Material if present
    if (material_)
    {
        material_->EnsureResourcesLoaded();
        if (auto matShader = material_->GetShader())
        {
            shader_ = matShader;
        }
    }

    // Bind texture (if any) to texture unit 0 and set uniforms
    std::shared_ptr<Texture> albedoTex = diffuse_texture;
    if (material_ && material_->GetAlbedoTexture())
    {
        albedoTex = material_->GetAlbedoTexture();
    }

    if (albedoTex && albedoTex->is_valid())
    {
        albedoTex->bind(GL_TEXTURE_2D, 0);
        shader_->use();
        // For Unlit mode, only uAlbedo is used; for Lit, both are used
        shader_->set_int("uAlbedo", 0);
        shader_->set_int("uUseTexture", 1);
    }
    else
    {
        shader_->use();
        shader_->set_int("uUseTexture", 0);
    }
    // Set ambient color if Lit and Scene available
    if (render_mode == RenderMode::Lit && Owner() && Owner()->GetScene())
    {
        shader_->use();
        const glm::vec3 ambient = Owner()->GetScene()->GetAmbientColor();
        shader_->set_vec3("uAmbient", ambient * material_ambient_multiplier);
    }

    if (render_mode == RenderMode::Unlit)
    {
        // For unlit, ignore light uniforms and just draw mesh with shader using uMVP
        shader_->use();
        const GLint locMVP = shader_->get_uniform_location_cached("uMVP");
        shader_->set_mat4(locMVP, mvp);
        mesh_->Bind();
        mesh_->Draw();
    }
    else
    {
        // Set material uniforms expected by the lit shader
        shader_->use();
        const glm::vec3 colorToUse = material_ ? material_->color : color;
        const float smoothnessToUse = material_ ? material_->smoothness : smoothness;
        shader_->set_vec3("uColor", colorToUse);
        shader_->set_float("uSmoothness", smoothnessToUse);

        // Approximate a single view direction per object in object space
        const glm::mat4 invView = glm::inverse(view);
        const glm::vec3 cameraWorldPos = glm::vec3(invView * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        const glm::vec3 objectWorldPos = glm::vec3(model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        const glm::vec3 worldViewDir = glm::normalize(cameraWorldPos - objectWorldPos);
        const glm::vec3 objectViewDir = glm::normalize(glm::vec3(invModel * glm::vec4(worldViewDir, 0.0f)));
        shader_->set_vec3("uViewDir", objectViewDir);

        renderer.DrawMesh(*mesh_, *shader_, mvp, lightCount, lightDirs, lightColors);
    }
}

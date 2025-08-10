#include "skybox_renderer.h"
#include "renderer.h"
#include "transform.h"

#include <glad/glad.h>

static const char* kSkyVS = R"glsl(
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

static const char* kSkyFS = R"glsl(
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
        FragColor = vec4(c,1.0);
    }
)glsl";

std::shared_ptr<Mesh> SkyboxRenderer::GetOrCreateCubeMesh()
{
    static std::weak_ptr<Mesh> weak;
    if (auto shared = weak.lock()) return shared;

    const float positions[] = {
        -1,-1,-1,  1,-1,-1,  1, 1,-1, -1, 1,-1,
        -1,-1, 1,  1,-1, 1,  1, 1, 1, -1, 1, 1
    };
    const unsigned int indices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        4,0,3, 3,7,4,
        1,5,6, 6,2,1,
        4,5,1, 1,0,4,
        3,2,6, 6,7,3
    };
    std::vector<MeshVertex> verts;
    verts.reserve(8);
    for (int i = 0; i < 8; ++i)
    {
        MeshVertex v{};
        v.position = glm::vec3(positions[i*3+0], positions[i*3+1], positions[i*3+2]);
        v.normal = glm::vec3(0,0,0);
        v.uv = glm::vec2(0,0);
        verts.push_back(v);
    }
    std::vector<unsigned int> idx(indices, indices + 36);
    auto mesh = std::make_shared<Mesh>(verts, idx);
    weak = mesh;
    return mesh;
}

void SkyboxRenderer::EnsureShader()
{
    if (shader_.id() == 0)
    {
        shader_ = Shader(kSkyVS, kSkyFS);
    }
}

void SkyboxRenderer::OnRender(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view)
{
    if (!texture_ || !texture_->is_valid()) return;

    glm::mat4 v = view;
    v[3][0] = v[3][1] = v[3][2] = 0.0f;

    // Depth state: draw background without affecting scene depth
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    // Draw inside of cube
    GLboolean wasCullEnabled = glIsEnabled(GL_CULL_FACE);
    GLint oldCullFace = GL_BACK;
    if (wasCullEnabled) glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFace);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    EnsureShader();
    shader_.use();
    shader_.set_mat4("uProj", projection);
    shader_.set_mat4("uViewNoT", v);
    texture_->bind(GL_TEXTURE_2D, 0);
    shader_.set_int("uSky", 0);

    auto mesh = GetOrCreateCubeMesh();
    // Renderer::DrawMesh expects lighting uniforms; we bypass and draw directly since the sky shader differs.
    mesh->Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

    // Restore state
    glCullFace(oldCullFace);
    if (!wasCullEnabled) glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}



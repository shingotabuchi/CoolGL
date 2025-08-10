#include "scene.h"
#include "game_object.h"
#include "camera.h"
#include "light.h"
#include "renderer.h"
#include "texture_loader.h"

#include <SOIL2.h>

GameObject& Scene::CreateObject()
{
    auto obj = std::make_unique<GameObject>();
    GameObject& ref = *obj;
    ref.SetScene(this);
    objects_.push_back(std::move(obj));
    return ref;
}

void Scene::Update(float time_seconds)
{
    for (auto& obj : objects_)
    {
        obj->Update(time_seconds);
    }
}

void Scene::Render(Renderer& renderer)
{
    const Camera* activeCamera = active_camera_;
    if (!activeCamera)
    {
        // No camera: skip rendering to enforce 'spawn a camera to render'
        return;
    }

    const glm::mat4 projection = activeCamera->ProjectionMatrix();
    const glm::mat4 view = activeCamera->ViewMatrix();

    // Clear using sky color so sky acts as background
    renderer.BeginFrame(sky_clear_color_.r, sky_clear_color_.g, sky_clear_color_.b, 1.0f);

    // Render skybox first (depth writes off, depth func LEQUAL)
    if (sky_texture_ != 0)
    {
        RenderSky(projection, view);
    }

    for (auto& obj : objects_)
    {
        obj->Render(renderer, projection, view);
    }
}

bool Scene::SetSkyFromEquirect(const std::string& path)
{
    int w=0,h=0,channels=0;
    unsigned char* data = nullptr;
    // Try a few common relative roots so running from build/Debug works
    const char* prefixes[] = {"", "../", "../../", "../../../"};
    std::string resolved;
    for (const char* p : prefixes)
    {
        std::string candidate = std::string(p) + path;
        data = SOIL_load_image(candidate.c_str(), &w, &h, &channels, SOIL_LOAD_AUTO);
        if (data)
        {
            resolved = candidate;
            break;
        }
    }
    if (!data || w<=0 || h<=0)
    {
        if (data) SOIL_free_image_data(data);
        return false;
    }

    // Compute two averages: full image for ambient, and upper half for sky clear color
    const int stride = channels; // 3 or 4
    auto accumRange = [&](int x0,int y0,int x1,int y1)->glm::vec3{
        double r=0,g=0,b=0; int count=0;
        for(int y=y0;y<y1;++y){
            const unsigned char* row = data + (size_t)y*w*stride;
            for(int x=x0;x<x1;++x){
                const unsigned char* px = row + (size_t)x*stride;
                r += px[0];
                if (stride>=3){ g += px[1]; b += px[2]; } else { g += px[0]; b += px[0]; }
                ++count;
            }
        }
        double inv = 1.0 / (255.0 * (double)count);
        return glm::vec3((float)(r*inv),(float)(g*inv),(float)(b*inv));
    };

    glm::vec3 avg_all = accumRange(0,0,w,h);

    ambient_color_ = avg_all * 0.8f;
    sky_clear_color_ = avg_all;

    // Create GL texture
    if (sky_texture_ == 0)
    {
        glGenTextures(1, &sky_texture_);
    }
    glBindTexture(GL_TEXTURE_2D, sky_texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLenum fmt = (channels == 4) ? GL_RGBA : (channels == 1 ? GL_RED : GL_RGB);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    // no mipmaps for sky to avoid seams

    // now safe to free image data
    SOIL_free_image_data(data);

    EnsureSkyMeshAndShader();
    return true;
}

// Build cube buffers and sky shader if not ready
void Scene::EnsureSkyMeshAndShader()
{
    if (sky_vao_ == 0)
    {
        const float positions[] = {
            // positions
            -1,-1,-1,  1,-1,-1,  1, 1,-1, -1, 1,-1, // back
            -1,-1, 1,  1,-1, 1,  1, 1, 1, -1, 1, 1  // front
        };
        const unsigned int indices[] = {
            0,1,2, 2,3,0,
            4,5,6, 6,7,4,
            4,0,3, 3,7,4,
            1,5,6, 6,2,1,
            4,5,1, 1,0,4,
            3,2,6, 6,7,3
        };
        glGenVertexArrays(1, &sky_vao_);
        glGenBuffers(1, &sky_vbo_);
        glGenBuffers(1, &sky_ebo_);
        glBindVertexArray(sky_vao_);
        glBindBuffer(GL_ARRAY_BUFFER, sky_vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sky_ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

    if (sky_shader_.id() == 0)
    {
        static const char* kVS = R"glsl(
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
        static const char* kFS = R"glsl(
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
        sky_shader_ = Shader(kVS, kFS);
    }
}

void Scene::RenderSky(const glm::mat4& projection, const glm::mat4& view)
{
    if (sky_texture_ == 0) return;
    // remove translation from view
    glm::mat4 v = view;
    v[3][0] = v[3][1] = v[3][2] = 0.0f;

    // Disable depth writes so sky doesn't affect scene depth
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    // Draw inside of cube. Save and restore cull state in case user changes it
    GLboolean wasCullEnabled = glIsEnabled(GL_CULL_FACE);
    GLint oldCullFace = GL_BACK;
    if (wasCullEnabled)
    {
        glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFace);
    }
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    sky_shader_.use();
    sky_shader_.set_mat4("uProj", projection);
    sky_shader_.set_mat4("uViewNoT", v);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sky_texture_);
    sky_shader_.set_int("uSky", 0);
    glBindVertexArray(sky_vao_);
    // Ensure index buffer is bound (VAO captures it, but be explicit)
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);

    // Restore cull state
    glCullFace(oldCullFace);
    if (!wasCullEnabled)
    {
        glDisable(GL_CULL_FACE);
    }
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void Scene::RegisterCamera(Camera* camera)
{
    // First-come-first-serve; allow swapping by explicit unregister/register order
    if (!active_camera_)
    {
        active_camera_ = camera;
    }
}

void Scene::UnregisterCamera(Camera* camera)
{
    if (active_camera_ == camera)
    {
        active_camera_ = nullptr;
    }
}

void Scene::RegisterLight(Light* light)
{
    // Avoid duplicates
    for (auto* l : lights_)
    {
        if (l == light) return;
    }
    lights_.push_back(light);
}

void Scene::UnregisterLight(Light* light)
{
    lights_.erase(std::remove(lights_.begin(), lights_.end(), light), lights_.end());
}



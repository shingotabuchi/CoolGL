#include "renderer.h"

#include <glad/glad.h>
// Define the thread_local cache declared in header
thread_local Renderer::CachedLightState Renderer::s_cachedLightState{};

Renderer::Renderer()
{
    glEnable(GL_DEPTH_TEST);
    // Enable back-face culling for fewer fragment shader invocations
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::BeginFrame(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawMesh(const Mesh& mesh,
                        const Shader& shader,
                        const glm::mat4& mvp,
                        int light_count,
                        const glm::vec3* light_dirs_object_space,
                        const glm::vec3* light_colors)
{
    shader.use();
    const GLint locMVP = shader.get_uniform_location_cached("uMVP");
    const GLint locLightCount = shader.get_uniform_location_cached("uLightCount");
    const GLint locLightDirs0 = shader.get_uniform_location_cached("uLightDirs[0]");
    const GLint locLightColors0 = shader.get_uniform_location_cached("uLightColors[0]");
    shader.set_mat4(locMVP, mvp);

    // Avoid redundant uniform updates for lights when possible
    const GLuint currentProgram = shader.id();
    bool programChanged = (s_cachedLightState.program != currentProgram);
    bool countChanged = (s_cachedLightState.count != light_count) || programChanged;
    bool dirsChanged = programChanged || countChanged;
    bool colorsChanged = programChanged || countChanged;

    if (!programChanged && !countChanged)
    {
        // Compare arrays only up to light_count
        for (int i = 0; i < light_count && !dirsChanged; ++i)
        {
            const glm::vec3 diff = s_cachedLightState.dirs[i] - light_dirs_object_space[i];
            if (glm::dot(diff, diff) > 1e-12f)
            {
                dirsChanged = true;
            }
        }
        for (int i = 0; i < light_count && !colorsChanged; ++i)
        {
            const glm::vec3 diff = s_cachedLightState.colors[i] - light_colors[i];
            if (glm::dot(diff, diff) > 1e-12f)
            {
                colorsChanged = true;
            }
        }
    }

    if (countChanged)
    {
        shader.set_int(locLightCount, light_count);
        s_cachedLightState.count = light_count;
    }
    if (dirsChanged)
    {
        shader.set_vec3_array(locLightDirs0, light_dirs_object_space, light_count);
        for (int i = 0; i < light_count; ++i) s_cachedLightState.dirs[i] = light_dirs_object_space[i];
    }
    if (colorsChanged)
    {
        shader.set_vec3_array(locLightColors0, light_colors, light_count);
        for (int i = 0; i < light_count; ++i) s_cachedLightState.colors[i] = light_colors[i];
    }
    s_cachedLightState.program = currentProgram;

    mesh.Bind();
    mesh.Draw();
}



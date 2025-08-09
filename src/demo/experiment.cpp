#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "engine/ModelLoader.h"
#include "engine/Shader.h"
#include <iostream>

const char *vertexShaderSrc = R"glsl(
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

const char *fragmentShaderSrc = R"glsl(
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

void framebuffer_size_callback(GLFWwindow *w, int w_, int h)
{
    glViewport(0, 0, w_, h);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 800, "Cool GL", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Fail\n";
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "GLAD init failed\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Mesh mesh = ModelLoader::loadFirstMeshFromFile("resources/cat/cat.fbx");

    Shader shader(vertexShaderSrc, fragmentShaderSrc);
    shader.use();

    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 10000.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    shader.setVec3("uLightPos", lightPos);
    shader.setVec3("uLightColor", lightColor);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float t = glfwGetTime();
        model = glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 mvp = projection * view * model;
        shader.setMat4("uMVP", mvp);
        mesh.bind();
        mesh.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

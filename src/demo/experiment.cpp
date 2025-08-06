#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <soil2.h>
#include <iostream>

struct Vertex {
    glm::vec3 position;
    glm::vec2 uv;
};

const char *vertexShaderSrc = R"glsl(
    #version 410 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aUV;
    out vec2 vUV;
    void main() {
        gl_Position = vec4(aPos, 1.0);
        vUV = aUV;
    }
)glsl";

const char *fragmentShaderSrc = R"glsl(
    #version 410 core
    in vec2 vUV;
    out vec4 FragColor;
    uniform sampler2D tex;
    void main() {
        FragColor = texture(tex, vUV);
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

    float sqrt3 = 1.7320508075688772f; // sqrt(3)
    float sqrt3_2 = sqrt3 / 2.0f;      // sqrt(3) / 2
    float static_scale = 0.8f;         // Static scale factor for the triangle

    Vertex vertices[] =
    {
        {
            glm::vec3{-sqrt3_2, -0.5f, 0.0f},
            glm::vec2{0.0f, 1.0f}
        },
        {
            glm::vec3{sqrt3_2, -0.5f, 0.0f},
            glm::vec2{1.0f, 1.0f}
        },
        {
            glm::vec3{0.0f, 1.0f, 0.0f},
            glm::vec2{0.5f, 0.0f}
        }
    };

    for (auto &vertex : vertices)
    {
        vertex.position *= static_scale;
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int texW, texH, texChannels;
    unsigned char* data = SOIL_load_image(
        "resources/doge.jpg",
        &texW, &texH, &texChannels,
        SOIL_LOAD_AUTO
    );
    if (!data)
    {
        std::cerr << "SOIL2 failed: " << SOIL_last_result() << "\n";
        return -1;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texW, texH, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    SOIL_free_image_data(data);

    glUseProgram(shaderProgram);
    int texLoc = glGetUniformLocation(shaderProgram, "tex");
    glUniform1i(texLoc, 0);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <print>
#include <array>

constexpr auto vertexShaderSource = R"(
    #version 330 core
    
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;

    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

constexpr auto fragmentShaderSource = R"(
    #version 330 core

    out vec4 FragColor;

    in vec2 TexCoord;
    uniform sampler2D texture1;

    void main()
    {
        FragColor = texture(texture1, TexCoord);
    }
)";

[[nodiscard]]
bool tryCompileShaderWithLog(GLuint shaderID);

[[nodiscard]]
bool tryLinkProgramWithLog(GLuint programID);

constexpr auto quadVertices = std::array{
    -0.5f, -0.5f, 0.0f, 0.0f,
     0.5f, -0.5f, 1.0f, 0.0f,
     0.5f,  0.5f, 1.0f, 1.0f,

     0.5f,  0.5f, 1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f
};


int main() {
    if (!glfwInit()) {
        std::println("Failed to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    const auto* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    const int screenWidth  = videoMode->width;
    const int screenHeight = videoMode->height;

    const int windowWidth  = screenHeight / 2;
    const int windowHeight = screenHeight / 2;

    auto* window = glfwCreateWindow(screenHeight / 2, screenHeight / 2,
                                    "OpenGL + GLFW", nullptr, nullptr);
    if (window == NULL) {
        std::println("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    // Calculate center position
    int windowPosX = (screenWidth - windowWidth) / 2;
    int windowPosY = (screenHeight - windowHeight) / 2;

    // Center the window
    glfwSetWindowPos(window, windowPosX, windowPosY);

    glfwSetWindowAspectRatio(window, 1, 1);
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::println("Failed to initialize GLEW");
        glfwTerminate();
        return -1;
    }

    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

    if (!tryCompileShaderWithLog(vertexShader)) {
        glfwTerminate();
        return -1;
    }

    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

    if (!tryCompileShaderWithLog(fragmentShader)) {
        glfwTerminate();
        return -1;
    }

    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    if (!tryLinkProgramWithLog(shaderProgram)) {
        glfwTerminate();
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int textureWidth, textureHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load("crate.png", &textureWidth,
                                    &textureHeight, &nrChannels, 0);

    if (data) {
        auto format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth,
                     textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
    }
    else {
        std::println("Failed to load texture");
    }

    stbi_image_free(data);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);

        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &texture);

    glfwTerminate();
    return 0;
}

bool tryCompileShaderWithLog(GLuint shaderID) {
    glCompileShader(shaderID);
    GLint success = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        char log[1024];
        glGetShaderInfoLog(shaderID, sizeof(log), nullptr, log);
        std::println("Shader compilation failed:\n{}", log);
        return false;
    }
    return true;
}

bool tryLinkProgramWithLog(GLuint programID) {
    glLinkProgram(programID);
    GLint success = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        char log[1024];
        glGetProgramInfoLog(programID, sizeof(log), nullptr, log);
        std::println("Program linking failed:\n{}", log);
        return false;
    }
    return true;
}


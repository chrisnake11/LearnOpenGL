#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include "stb_image.h"
#include "Camera.h"
#include "Shader.h"
#include <memory>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void inputProcess(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
unsigned int loadTexture(const char* path);

constexpr int SCR_WIDTH = 800;
constexpr int SCR_HEIGHT = 600;

Camera camera{glm::vec3(0.5f, 1.0f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -100.0f, -15.0f}; // initial position

// cursor in center initially
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

constexpr auto lightColor = glm::vec3(0.3f, 0.7f, 0.25f);
constexpr auto lightPos = glm::vec3(-1.0f, 1.3f, -3.0f);
constexpr auto lightConstant = 1.0f;
constexpr auto lightLinear = 0.09f;
constexpr auto lightQuadratic = 0.032f;
constexpr auto lightCutOff = glm::radians(12.5f);
constexpr auto lightOuterCutOff = glm::radians(15.0f);
constexpr auto lightDir = glm::vec3(-0.2f, -1.0f, -0.3f);

// ambient lighting parameters
float ambientLightIntensity = 0.1f;

float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

int main(){
    if(!glfwInit()){
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if(!window){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    const float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,


        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
    };

    std::vector<glm::vec3> cubePositions;
    for (float x = -3.0f; x <= 3.0f; x += 1.5f) {
        for (float y = -3.0f; y <= 3.0f; y += 1.5f) {
                cubePositions.emplace_back(x, y, 0.0f);
        }
    }


    // load shader
    //
    auto colorVertexPath = "./color.vs";
    auto colorFragmentPath = "./color.fs";
    const auto colorShader = std::make_unique<Shader>(colorVertexPath, colorFragmentPath);

    const auto lightVertexPath = "./light.vs";
    const auto lightFragmentPath = "./light.fs";
    const auto lightShader = std::make_unique<Shader>(lightVertexPath, lightFragmentPath);

    // set up vertex data, buffers, and configure vertex attributes
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    // load image, create texture and generate mipmaps
    const unsigned int diffuseMap = loadTexture("./woodbox.png");
    const unsigned int specularMap = loadTexture("./woodbox_specular.png");

    colorShader->use();
    colorShader->setInt("material.diffuse", 0);
    colorShader->setInt("material.specular", 1);

    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(window)){

        const auto currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        inputProcess(window);

        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // lightPos = glm::vec3(1.0f * sin(glfwGetTime()), 0.5f, 3.0f * cos(glfwGetTime()));

        colorShader->use();
        // set point-light properties
        colorShader->setVec3("pointLight.position", lightPos);
        colorShader->setVec3("pointLight.ambient", glm::vec3(0.9f, 0.9f, 0.9f));
        colorShader->setVec3("pointLight.diffuse", glm::vec3(0.3f, 0.7f, 0.25f));
        colorShader->setVec3("pointLight.specular", glm::vec3(0.3f, 0.7f, 0.25f));
        colorShader->setFloat("pointLight.constant", lightConstant);
        colorShader->setFloat("pointLight.linear", lightLinear);
        colorShader->setFloat("pointLight.quadratic", lightQuadratic);

        // set camera spot-light properties
        colorShader->setVec3("spotLight.position", camera.Position);
        colorShader->setVec3("spotLight.direction", camera.Front);
        colorShader->setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        colorShader->setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        colorShader->setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        colorShader->setFloat("spotLight.constant", lightConstant);
        colorShader->setFloat("spotLight.linear", lightLinear);
        colorShader->setFloat("spotLight.quadratic", lightQuadratic);
        colorShader->setFloat("spotLight.cutOff", glm::cos(lightCutOff));
        colorShader->setFloat("spotLight.outerCutOff", glm::cos(lightOuterCutOff));

        //set dir-light properties
        colorShader->setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        colorShader->setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        colorShader->setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        colorShader->setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        colorShader->setVec3("viewPos", camera.Position);
        auto projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        colorShader->setMat4("projection", projection);
        auto view = camera.GetViewMatrix();
        colorShader->setMat4("view", view);
        colorShader->setFloat("material.shininess", 32.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(cubeVAO);
        for (const auto position : cubePositions) {
            auto model = glm::mat4(1.0f);
            model = translate(model, position);
            colorShader->setMat4("model", model);
            // calculate normal matrix
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
            colorShader->setMat3("normalMatrix", normalMatrix);
            // render the cube
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        lightShader->use();
        lightShader->setMat4("projection", projection);
        lightShader->setMat4("view", view);
        auto model = glm::mat4(1.0f);
        model = translate(model, lightPos);
        model = scale(model, glm::vec3(0.1f)); // a smaller cube
        lightShader->setMat4("model", model);
        lightShader->setVec3("lightColor", lightColor);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height){
    glViewport(0, 0, width, height);
}

void inputProcess(GLFWwindow* window){
    // esc, show cursor
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // focus window, hide cursor
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // move camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouseCallback(GLFWwindow* window, const double xPos, const double yPos){
    if(firstMouse){
        lastX = static_cast<float>(xPos);
        lastY = static_cast<float>(yPos);
        firstMouse = false;
    }

    const float xOffset = static_cast<float>(xPos) - lastX;
    const float yOffset = lastY - static_cast<float>(yPos); // reversed since y-coordinates go from bottom to top

    lastX = static_cast<float>(xPos);
    lastY = static_cast<float>(yPos);

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, const double xOffset, const double yOffset){
    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width = 0, height = 0, nrComponents = 0;
    if (unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0))
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // NOLINT(*-narrowing-conversions)
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
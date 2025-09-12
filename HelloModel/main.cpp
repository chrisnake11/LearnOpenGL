#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include "stb_image.h"
#include "Camera.h"
#include "Shader.h"
#include <memory>
#include <vector>
#include "Model.h"

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

constexpr auto lightColor = glm::vec3(1.0f);
constexpr auto lightPos = glm::vec3(-3.0f, 3.3f, -7.0f);
constexpr auto lightConstant = 1.0f;
constexpr auto lightLinear = 0.09f;
constexpr auto lightQuadratic = 0.032f;
constexpr auto lightCutOff = glm::radians(13.0f);
constexpr auto lightOuterCutOff = glm::radians(17.0f);

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

    // load shader
    auto colorVertexPath = "./color.vs";
    auto colorFragmentPath = "./color.fs";
    const auto colorShader = std::make_unique<Shader>(colorVertexPath, colorFragmentPath);

    auto outLineVertexPath = "./outline.vs";
    auto outLineFragmentPath = "./outline.fs";
    const auto outLineShader = std::make_unique<Shader>(outLineVertexPath, outLineFragmentPath);

    colorShader->use();

    const Model ourModel("./backpack.obj");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    while (!glfwWindowShouldClose(window)){

        const auto currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        inputProcess(window);



        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        colorShader->use();
        colorShader->setVec3("spotLight.position", camera.Position);
        colorShader->setVec3("spotLight.direction", camera.Front);
        colorShader->setVec3("spotLight.lightColor", lightColor);
        colorShader->setVec3("spotLight.ambient", glm::vec3(0.2f));
        colorShader->setVec3("spotLight.diffuse", glm::vec3(0.4f));
        colorShader->setVec3("spotLight.specular", glm::vec3(0.4f));
        colorShader->setFloat("spotLight.constant", lightConstant);
        colorShader->setFloat("spotLight.linear", lightLinear);
        colorShader->setFloat("spotLight.quadratic", lightQuadratic);
        colorShader->setFloat("spotLight.cutOff", glm::cos(lightCutOff));
        colorShader->setFloat("spotLight.outerCutOff", glm::cos(lightOuterCutOff));
        colorShader->setVec3("viewPos", camera.Position);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        colorShader->setMat4("projection", projection);
        colorShader->setMat4("view", view);
        colorShader->setMat4("model", model);

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        ourModel.draw(*colorShader);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        glm::mat4 scaleModel = glm::scale(model, glm::vec3(1.05f));
        outLineShader->use();
        outLineShader->setMat4("projection", projection);
        outLineShader->setMat4("view", view);
        outLineShader->setMat4("scaleModel", scaleModel);
        ourModel.draw(*outLineShader);
        glEnable(GL_DEPTH_TEST);
        glStencilMask(0xFF);

        glBindVertexArray(0);
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
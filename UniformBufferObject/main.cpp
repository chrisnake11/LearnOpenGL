#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include "Camera.h"
#include "Shader.h"
#include <memory>
#include <vector>

GLFWwindow* initMainWindow();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void inputProcess(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);


constexpr int SCR_WIDTH = 800;
constexpr int SCR_HEIGHT = 600;

Camera camera{glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -100.0f, -15.0f}; // initial position

// cursor in center initially
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

int main(){
    GLFWwindow* window = nullptr;
    window = initMainWindow();
    if (window == nullptr) return -1;

    glEnable(GL_DEPTH_TEST);

    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
    };

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    Shader redShader("./advanceGlsl.vs", "./red.fs");
    Shader greenShader("./advanceGlsl.vs", "./green.fs");
    Shader blueShader("./advanceGlsl.vs", "./blue.fs");
    Shader yellowShader("./advanceGlsl.vs", "./yellow.fs");

    unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(redShader.ID, "Matrices");
    unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(greenShader.ID, "Matrices");
    unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(blueShader.ID, "Matrices");
    unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(yellowShader.ID, "Matrices");
    // link each shader's uniform block to binding point 0
    glUniformBlockBinding(redShader.ID, uniformBlockIndexRed, 0);
    glUniformBlockBinding(greenShader.ID, uniformBlockIndexGreen, 0);
    glUniformBlockBinding(blueShader.ID, uniformBlockIndexBlue, 0);
    glUniformBlockBinding(yellowShader.ID, uniformBlockIndexYellow, 0);

    // set up matrices uniform buffer object
    unsigned int matricesUBO;
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, matricesUBO, 0, 2*sizeof(glm::mat4));

    while (!glfwWindowShouldClose(window)){
        const auto currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        inputProcess(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set projection matrix once
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                                0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);


        glBindVertexArray(cubeVAO);

        glm::mat4 model = glm::mat4(1.0f);
        redShader.use();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.0f, 0.0f));
        redShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        greenShader.use();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 0.0f));
        greenShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        blueShader.use();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, 0.0f));
        blueShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        yellowShader.use();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -2.0f, 0.0f));
        yellowShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow* initMainWindow() {
    if(!glfwInit()){
        std::cout << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);


    if(!window){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    return window;
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
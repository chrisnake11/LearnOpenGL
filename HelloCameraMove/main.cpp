#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Shader.h"
#include "stb_image.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <cmath>
#include <vector>
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera/view transformation
Camera camera(glm::vec3(0.0f, 0.0f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

// mouse last position
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f; // time of last frame

int main()
{

    // 初始化GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello Triangle", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // 设置回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    
    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    const char *vertexShaderPath = "./vs.glsl";
    const char *fragmentShaderPath = "./fs.glsl";
    std::unique_ptr<Shader> ourShader = std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath);

    // single cube vertices with texture coordinates
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    // 3 * 3 * 3 cubes
    std::vector<glm::vec3> cubePositions;
    for (int z = -2; z <= 2; z += 2)
    {
        for (int y = -2; y <= 2; y += 2)
        {
            for (int x = -2; x <= 2; x += 2)
            {
                cubePositions.push_back(glm::vec3((float)x, (float)y, (float)z));
            }
        }
    }

    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    // 绑定VAO，后续的VBO和属性配置都绑定到这个VAO上
    glBindVertexArray(VAO);

    // 绑定VBO到状态机，并拷贝数据到GPU显存
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 第一个属性，每次间隔5个floats，偏移量为0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 第二个属性，每次间隔5个floats，偏移量为3个floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // gen texture vertices and bind to state machine
    unsigned int texture_container;
    glGenTextures(1, &texture_container);
    glBindTexture(GL_TEXTURE_2D, texture_container);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // set texture filtering to process texels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    int width, height, nrChannels;
    const char *avatarPath = "./avatar.png";
    // load avatar.png information
    // flip the image vertically. opengl 0.0 y coordinate is at the bottom of the image, but images usually have 0.0 at the top of the image
    stbi_set_flip_vertically_on_load(true);
    unsigned char *avatar_data = stbi_load(avatarPath, &width, &height, &nrChannels, 0);

    std::cout << "avatar.png path: " << avatarPath << std::endl;
    if (avatar_data)
    {
        // load texture data to GPU
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, avatar_data);
        std::cout << "avatar.png width: " << width << ", height: " << height << ", nrChannels: " << nrChannels << std::endl;
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << avatarPath << std::endl;
    }

    stbi_image_free(avatar_data);

    unsigned int texture_face;
    glGenTextures(1, &texture_face);
    glBindTexture(GL_TEXTURE_2D, texture_face);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // set texture filtering to process texels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    const char *awesomefacePath = "./awesomeface.png";
    unsigned char *face_data = stbi_load(awesomefacePath, &width, &height, &nrChannels, 0);

    std::cout << "awesomeface.png path: " << awesomefacePath << std::endl;
    if (face_data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, face_data);
        std::cout << "awesomeface.png width: " << width << ", height: " << height << ", nrChannels: " << nrChannels << std::endl;
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << awesomefacePath << std::endl;
    }
    stbi_image_free(face_data);

    ourShader->use();
    std::unique_ptr<ShaderDataAdapter<int>> intAdapter = std::make_unique<ShaderDataAdapter<int>>();
    setData(ourShader.get(), intAdapter.get(), "texture1", 0);
    setData(ourShader.get(), intAdapter.get(), "texture2", 1);

    glEnable(GL_DEPTH_TEST);


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        // background color
        float timeValue = (float)glfwGetTime();
        float pi = 3.14159265358979323846f;
        glClearColor(sin(timeValue * pi), sin(timeValue * pi + (pi / 2)), sin(timeValue * pi + pi), 0.8f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 添加一个纹理单元到状态机，并绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_container);
        // 添加另一个纹理单元到状态机，并绑定纹理
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_face);

        ourShader->use();
        glBindVertexArray(VAO);

        // projection
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        unsigned int projectionLoc = glGetUniformLocation(ourShader->ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // camera view
        glm::mat4 view = camera.GetViewMatrix();
        unsigned int viewLoc = glGetUniformLocation(ourShader->ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        for (int i = 0; i < cubePositions.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            unsigned int modelLoc = glGetUniformLocation(ourShader->ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // draw the rectangle
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
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

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    // save the last mouse position
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

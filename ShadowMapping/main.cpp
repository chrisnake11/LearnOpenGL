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
#include "Mesh.h"


GLFWwindow* initMainWindow();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void inputProcess(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
unsigned int loadTexture(const char* path);
unsigned int loadCubeMap(const std::vector<std::string>& faces);

constexpr int SCR_WIDTH = 800;
constexpr int SCR_HEIGHT = 600;

Camera camera{glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -100.0f, -15.0f}; // initial position

glm::vec3 lightPos{-20.0f, 50.0f, 30.0f};

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
    glDepthFunc(GL_LESS);

    Shader skyboxShader("./shaders/skybox.vs", "./shaders/skybox.fs");
    Shader ourShader("./shaders/kati.vs", "./shaders/kati.fs");
    Shader reflectShader("./shaders/reflectShader.vs", "./shaders/reflectShader.fs");
    Shader refractShader("./shaders/refractShader.vs", "./shaders/refractShader.fs");
    Shader floorShader("./shaders/floor.vs", "./shaders/floor.fs");
    Shader shadowShader("./shaders/shadowShader.vs", "./shaders/shadowShader.fs");

    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    float planeVertices[] = {
        // positions            // normals         // texcoords
        100.0f, 0.0f,  100.0f,  0.0f, 1.0f, 0.0f,  100.0f,  0.0f,
       -100.0f, 0.0f,  100.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
       -100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f,   0.0f, 100.0f,

        100.0f, 0.0f,  100.0f,  0.0f, 1.0f, 0.0f,  100.0f,  0.0f,
       -100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f,   0.0f, 100.0f,
        100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f,  100.0f, 100.0f
   };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    const std::vector<std::string> faces = {
        "./resources/right.jpg",
        "./resources/left.jpg",
        "./resources/top.jpg",
        "./resources/bottom.jpg",
        "./resources/front.jpg",
        "./resources/back.jpg"
    };

    // load textures
    stbi_set_flip_vertically_on_load(false);
    unsigned int skyboxTexture = loadCubeMap(faces);

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int floorTexture = loadTexture("./resources/wood.png");

    floorShader.use();
    floorShader.setInt("diffuseTexture", 0); // 纹理单元0
    floorShader.setInt("shadowMap", 1); // 纹理单元1

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT,GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // set up skybox shader
    skyboxShader.use();
    skyboxShader.setInt("skybox", 2); // 纹理单元2

    const Model ourModel("./resources/kati.pmx"); // 纹理单元(0, 1)表示diff和reflect

    reflectShader.use();
    reflectShader.setInt("skybox", 2);

    refractShader.use();
    refractShader.setInt("skybox", 2);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)){
        const auto currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        lightPos = glm::vec3(70.0f * sin(currentTime), 50.0f, 70.0f * cos(currentTime));

        inputProcess(window);

        /*
         * 1. 从光源视角，渲染场景（Vertex）到深度贴图缓冲中。
         */
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        // render depth map first
        shadowShader.use();
        glm::mat4 lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, 1.0f, 200.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // place models
        // floor
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.001f, 0.0f));
        shadowShader.setMat4("model", model);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        // three kati model
        model = glm::mat4(1.0f);
        shadowShader.setMat4("model", model);
        ourModel.draw(shadowShader);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 0.0f, 0.0f));
        shadowShader.setMat4("model", model);
        ourModel.draw(shadowShader);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 0.0f, 0.0f));
        shadowShader.setMat4("model", model);
        ourModel.draw(shadowShader);
        glCullFace(GL_BACK);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*
         * 2. 正常渲染场景，包括材质贴图等内容。
         */
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        model = glm::mat4(1.0f);
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);
        ourShader.setVec3("viewPos", camera.Position);
        // bind skybox texture to texture unit 2 before model bind other texture
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        // draw kati model
        ourModel.draw(ourShader);

        // draw another
        reflectShader.use();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 0.0f, 0.0f));
        reflectShader.setMat4("projection", projection);
        reflectShader.setMat4("view", view);
        reflectShader.setMat4("model", model);
        reflectShader.setVec3("viewPos", camera.Position);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        ourModel.draw(reflectShader);

        // draw another
        refractShader.use();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 0.0f, 0.0f));
        refractShader.setMat4("projection", projection);
        refractShader.setMat4("view", view);
        refractShader.setMat4("model", model);
        refractShader.setVec3("viewPos", camera.Position);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        ourModel.draw(refractShader);

        // draw floor
        floorShader.use();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.001f, 0.0f));
        floorShader.setMat4("projection", projection);
        floorShader.setMat4("view", view);
        floorShader.setMat4("model", model);
        floorShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        floorShader.setVec3("lightPos", lightPos);
        floorShader.setVec3("viewPos", camera.Position);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // draw skybox
        glDepthFunc(GL_LEQUAL); // skybox depth trick, depth = 1.0;
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // add an offset to sync skybox sun position and light position
        float angle_offset = currentTime + glm::pi<float>();
        model = glm::rotate(glm::mat4(1.0f), angle_offset, glm::vec3(0.0f, 1.0f, 0.0f));
        skyboxShader.setMat4("model", model);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

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

unsigned int loadTexture(const char *path) {

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = nullptr;
    data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        // set the texture wrapping parameters
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set texture filtering to process texels
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

unsigned int loadCubeMap(const std::vector<std::string>& faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    unsigned char * data = nullptr;
    for (unsigned int i  = 0; i < faces.size(); i++) {
        data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            std::cout << "Failed to load texture at path: " << faces[i] << std::endl;
            stbi_image_free(data);
            continue;
        }
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
        );
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
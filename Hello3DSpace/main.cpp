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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(){

    // 初始化GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    const char* vertexShaderPath = "D:/dev/ComputerGraphics/LearnOpenGL/Hello3DSpace/vs.glsl";
    const char* fragmentShaderPath = "D:/dev/ComputerGraphics/LearnOpenGL/Hello3DSpace/fs.glsl";
    std::unique_ptr<Shader> ourShader = std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath);

    // single cube vertices with texture coordinates
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    
    // 3 * 3 * 3 cubes
    std::vector<glm::vec3> cubePositions;
    for(int z = -2; z <= 2; z += 2){
        for(int y = -2; y <= 2; y += 2){
            for(int x = -2; x <= 2; x += 2){
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 第二个属性，每次间隔5个floats，偏移量为3个floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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
    const char* avatarPath = "D:/dev/ComputerGraphics/LearnOpenGL/Hello3DSpace/avatar.png";
    // load avatar.png information
    // flip the image vertically. opengl 0.0 y coordinate is at the bottom of the image, but images usually have 0.0 at the top of the image
    stbi_set_flip_vertically_on_load(true);
    unsigned char *avatar_data = stbi_load(avatarPath, &width, &height, &nrChannels, 0);

    std::cout << "avatar.png path: " << avatarPath << std::endl;
    if(avatar_data){
        // load texture data to GPU
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, avatar_data);
        std::cout << "avatar.png width: " << width << ", height: " << height << ", nrChannels: " << nrChannels << std::endl;
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
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

    const char* awesomefacePath = "D:/dev/ComputerGraphics/LearnOpenGL/Hello3DSpace/awesomeface.png";
    unsigned char *face_data = stbi_load(awesomefacePath, &width, &height, &nrChannels, 0);
    
    std::cout << "awesomeface.png path: " << awesomefacePath << std::endl;
    if(face_data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, face_data);
        std::cout << "container.jpg width: " << width << ", height: " << height << ", nrChannels: " << nrChannels << std::endl;
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture: " << awesomefacePath << std::endl;
    }
    stbi_image_free(face_data);

    ourShader->use();
    std::unique_ptr<ShaderDataAdapter<int>> intAdapter = std::make_unique<ShaderDataAdapter<int>>();
    setData(ourShader.get(), intAdapter.get(), "texture1", 0);
    setData(ourShader.get(), intAdapter.get(), "texture2", 1);

    glEnable(GL_DEPTH_TEST); 

    while(!glfwWindowShouldClose(window)) {
        // rotate by x axis
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        const float radius = 10.0f;
        float camX = sin((float)glfwGetTime()) * radius;
        float camZ = cos((float)glfwGetTime()) * radius;
        // x-z plane circle
        glm::vec3 cameraPos = glm::vec3(camX, 0.0f, camZ);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        // perspective projection
        glm::mat4 projection = glm::mat4(1.0f);
        // fov, aspect ratio, near plane, far plane
        projection = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        
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

        // pass transformation matrices to the shader
        unsigned int modelLoc = glGetUniformLocation(ourShader->ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int viewLoc = glGetUniformLocation(ourShader->ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        unsigned int projectionLoc = glGetUniformLocation(ourShader->ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // draw the rectangle

        for (int i = 0; i < cubePositions.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f;
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f) + glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
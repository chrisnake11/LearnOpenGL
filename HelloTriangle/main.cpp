#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

struct Offset{
    float x;
    float y;
};

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

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    std::string vertexShaderSource;

    auto getSource = [](std::string& source, const std::string& path){
        // read vertex shader code from file
        std::ifstream vertexShaderFile(path);
        if (vertexShaderFile) {
            std::stringstream buffer;
            buffer << vertexShaderFile.rdbuf();
            source = buffer.str();
        } else {
            std::cerr << "Failed to open shader file." << std::endl;
        }
    };

    getSource(vertexShaderSource, "d:/dev/ComputerGraphics/LearnOpenGL/HelloTriangle/vs.glsl");

    // create vertex shader object
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // load shader source code into the shader object and compile it
    const char* vertexShaderSourceCStr = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSourceCStr, nullptr);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    std::string fragmentShaderSource;
    getSource(fragmentShaderSource, "d:/dev/ComputerGraphics/LearnOpenGL/HelloTriangle/fs.glsl");

    // create fragment shader object
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, nullptr);
    glCompileShader(fragmentShader);

    // create shader program object
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // delete the shader objects once we've linked them into the program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };


    /* create and bind the buffer object */
    unsigned int VBO, VAO;
    unsigned int EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    // bind the buffer object to gl_array_buffer
    // copy vertices data to buffer's memory
    // 配置顶点属性指针
    glBindVertexArray(VAO);
    // 拷贝顶点数据到GPU显存
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 拷贝顶点索引数据到GPU显存
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 将顶点属性与顶点数据关联起来。
    // 第一个参数指定顶点属性的索引为0。第二个参数指定顶点为vec3，第三个参数指定顶点坐标为float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // 启用索引为0的顶点属性设置
    glEnableVertexAttribArray(0);

    // 顶点属性和顶点数据关联后，可以解绑VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 后续需要用索引绘制的话，不能解绑EBO
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // 全局xy轴的偏移量
    Offset offset = {0.0f, 0.0f};
    while (!glfwWindowShouldClose(window)) {
        // listen for events
        glfwPollEvents();
        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // 当按下wasd时，累积偏移量
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            offset.y += 0.001f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            offset.y -= 0.001f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            offset.x -= 0.001f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            offset.x += 0.001f;

        // fill the background color after clearing
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
        // clear color buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        // 查找名为offset的uniform变量位置
        int offsetLoc = glGetUniformLocation(shaderProgram, "offset");
        // 设置offset变量的值
        glUniform2f(offsetLoc, offset.x, offset.y);

        // bind the vertex array object
        glBindVertexArray(VAO);
        
        // draw the triangles
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        // display framebuffer
        glfwSwapBuffers(window); // swap colors in the back buffer to front buffer
    }

    // optional: de-allocate all resources once they've outlived their purpose.
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

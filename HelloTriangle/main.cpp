#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


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
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 拷贝顶点数据到GPU显存
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // 拷贝顶点索引数据到GPU显存
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 将顶点属性与顶点数据关联起来
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // enable the vertex attribute array
    glEnableVertexAttribArray(0);

    // 顶点属性和顶点数据关联后，可以解绑VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 后续需要用索引绘制的话，不能解绑EBO
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window)) {
        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // fill the background color after clearing
        glClear(GL_COLOR_BUFFER_BIT); // GL_COLOR_BUFFER_BIT means clear the color buffer 
        
        // use the shader program to render
        glUseProgram(shaderProgram);
        
        // bind the vertex array object
        glBindVertexArray(VAO);
        
        // draw the triangles
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        // display framebuffer
        glfwSwapBuffers(window); // swap colors in the back buffer to front buffer

        // listen for events
        glfwPollEvents();

    }

    // optional: de-allocate all resources once they've outlived their purpose.
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

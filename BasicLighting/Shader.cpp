#include "Shader.h"
#include <glm.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    
    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
    
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
    
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

void Shader::use() 
{ 
    glUseProgram(ID);
}

void ShaderDataAdapter<float>::setData(Shader* shader, const std::string &name , const float& value) const{
    glUniform1f(glGetUniformLocation(shader->ID, name.c_str()), value);
}

void ShaderDataAdapter<int>::setData(Shader* shader, const std::string &name , const int& value) const{
    glUniform1i(glGetUniformLocation(shader->ID, name.c_str()), value);
}

void ShaderDataAdapter<bool>::setData(Shader* shader, const std::string &name , const bool& value) const{
    glUniform1i(glGetUniformLocation(shader->ID, name.c_str()), (int)value);
}

void ShaderDataAdapter<glm::vec2>::setData(Shader* shader, const std::string &name , const glm::vec2& value) const{
    glUniform2f(glGetUniformLocation(shader->ID, name.c_str()), value.x, value.y);
}

void ShaderDataAdapter<glm::vec3>::setData(Shader* shader, const std::string &name , const glm::vec3& value) const{
    glUniform3f(glGetUniformLocation(shader->ID, name.c_str()), value.x, value.y, value.z);
}

void ShaderDataAdapter<glm::vec4>::setData(Shader* shader, const std::string &name , const glm::vec4& value) const{
    glUniform4f(glGetUniformLocation(shader->ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void ShaderDataAdapter<glm::mat4>::setData(Shader* shader, const std::string &name , const glm::mat4& value) const{
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void ShaderDataAdapter<Material>::setData(Shader* shader, const std::string &name , const Material& value) const{
    glUniform3f(glGetUniformLocation(shader->ID, (name + ".ambient").c_str()), value.ambient.r, value.ambient.g, value.ambient.b);
    glUniform3f(glGetUniformLocation(shader->ID, (name + ".diffuse").c_str()), value.diffuse.r, value.diffuse.g, value.diffuse.b);
    glUniform3f(glGetUniformLocation(shader->ID, (name + ".specular").c_str()), value.specular.r, value.specular.g, value.specular.b);
    glUniform1f(glGetUniformLocation(shader->ID, (name + ".shininess").c_str()), value.shininess);
}
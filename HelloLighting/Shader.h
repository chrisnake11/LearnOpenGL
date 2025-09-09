#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
class Shader{
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
};

template<typename T>
class ShaderDataAdapter{
public:
    virtual void setData(Shader* shader, const std::string &name , const T& value) const = 0;
};

template<>
class ShaderDataAdapter<float>{
public:
    void setData(Shader* shader, const std::string &name , const float& value) const;
};

template<>
class ShaderDataAdapter<int>{
public:
    void setData(Shader* shader, const std::string &name , const int& value) const;
};

template<>
class ShaderDataAdapter<bool>{
public:
    void setData(Shader* shader, const std::string &name , const bool& value) const;
};

template<>
class ShaderDataAdapter<glm::vec2>{
public:
    void setData(Shader* shader, const std::string &name , const glm::vec2& value) const;
};

template<>
class ShaderDataAdapter<glm::vec3>{
public:
    void setData(Shader* shader, const std::string &name , const glm::vec3& value) const;
};

template<>
class ShaderDataAdapter<glm::vec4>{
public:
    void setData(Shader* shader, const std::string &name , const glm::vec4& value) const;
};

template<>
class ShaderDataAdapter<glm::mat4>{
public:
    void setData(Shader* shader, const std::string &name , const glm::mat4& value) const;
};

template<typename T>
void setData(Shader* shader, ShaderDataAdapter<T>* adapter, const std::string &name , const T& value){
    adapter->setData(shader, name, value);
}
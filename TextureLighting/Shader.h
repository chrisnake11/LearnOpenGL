#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

struct Material{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

class Shader{
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    void setBool(const std::string& name, bool value) const;
    void setVec2(const std::string& name, const glm::vec2 &value) const;
    void setVec3(const std::string& name, const glm::vec3 &value) const;
    void setVec4(const std::string& name, const glm::vec4 &value) const;
    void setMat4(const std::string& name, const glm::mat4 &mat) const;

    template<typename T>
    void setData(const std::string &name , const T& value) const;
};

template<typename T>
class ShaderDataAdapter{
public:
    void setData(Shader* shader, const std::string &name , const T& value) const = 0;
};

template<>
class ShaderDataAdapter<Material>{
public:
    void setData(Shader* shader, const std::string &name , const Material& value) const;
};

template <typename T>
void Shader::setData(const std::string &name , const T& value) const{
    ShaderDataAdapter<T>  adapter;
    adapter.setData(this, name, value);
}
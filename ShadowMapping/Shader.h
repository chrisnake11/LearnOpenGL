#pragma once
#include <glm/glm.hpp>
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
    void setMat3(const std::string& name, const glm::mat3 &mat) const;
    void setMat4(const std::string& name, const glm::mat4 &mat) const;


    template<typename T>
    void setData(const std::string &name , const T& value) const;
};



template<typename T>
class ShaderDataAdapter{
public:
    static void setData(Shader *shader, const std::string &name, const T &value) = delete;
};

template<>
class ShaderDataAdapter<Material>{
public:
    static void setData(const Shader* shader, const std::string &name , const Material& value);
};

template<typename T>
void Shader::setData(const std::string &name, const T &value) const {
    ShaderDataAdapter<T>::setData(const_cast<Shader*>(this), name, value);
}
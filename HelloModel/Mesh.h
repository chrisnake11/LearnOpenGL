//
// Created by zyx on 2025/9/11.
//

#ifndef MAIN_MESH_H
#define MAIN_MESH_H
#include <string>
#include <vector>
#include "Shader.h"
#include <glm/glm.hpp>

// vertex data and texture uv coordinates
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords; // (u, v) texture coordinates
};

// texture picture data
struct Texture {
    unsigned int id;
    std::string type;
    std::string path; // for texture deduplication
};

// a mesh is a collection of vertices, indices and textures
// each vertex bind to a texture uv coordinate
class Mesh {
public:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void draw(const Shader &shader) const;
private:
    unsigned int m_VAO{}, m_VBO{}, m_EBO{};
    void setupMesh();
};

#endif //MAIN_MESH_H
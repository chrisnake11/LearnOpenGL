//
// Created by zyx on 2025/9/11.
//

#include "Mesh.h"
#include <glad/glad.h>
#include <iostream>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) :
    m_vertices(std::move(vertices)), m_indices(std::move(indices)), m_textures(std::move(textures)) {
    setupMesh();
}

void Mesh::setupMesh() {
    // create buffers/arrays
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    // copy buffer data to gpu
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(m_vertices[0]), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(m_indices[0]), &m_indices[0], GL_STATIC_DRAW);

    // bind vertex attributes to buffer data
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, texCoords)));
}

void Mesh::draw(const Shader &shader) const {
    unsigned int diffuseNr = 1;
    unsigned int reflectionNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for(unsigned int i = 0; i < m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string name = m_textures[i].type;
        std::string uniformName;
        if (name == "texture_diffuse") {
            uniformName = "texture_diffuse[" + std::to_string(diffuseNr++) + "]";
        } else if (name == "texture_specular") {
            uniformName = "texture_specular[" + std::to_string(specularNr++) + "]";
        } else if (name == "texture_normal") {
            uniformName = "texture_normal[" + std::to_string(normalNr++) + "]";
        } else if (name == "texture_height") {
            uniformName = "texture_height[" + std::to_string(heightNr++) + "]";
        } else if (name == "texture_reflection") {
            uniformName = "texture_reflection[" + std::to_string(reflectionNr++) + "]";
        }

        // 绑定到对应纹理单元
        shader.setInt(uniformName, i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }
    shader.setInt("numDiffuse", diffuseNr - 1);
    shader.setInt("numReflection", reflectionNr - 1);
    shader.setInt("numSpecular", specularNr - 1);
    shader.setInt("numNormal", normalNr - 1);
    shader.setInt("numHeight", heightNr - 1);

    // draw the mesh(all the triangles)
    glBindVertexArray(m_VAO);
    // draw all the
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}




//
// Created by zyx on 2025/9/11.
//

#include "Mesh.h"
#include <glad/glad.h>

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
    // unsigned int specularNr = 1;
    // unsigned int normalNr = 1;
    // unsigned int heightNr = 1;
    for(unsigned int i = 0; i < m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number{};
        std::string name = m_textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_reflection")
            number = std::to_string(reflectionNr++); // only one reflection map
        // else if (name == "texture_specular")
        //     number = std::to_string(specularNr++);
        // else if (name == "texture_normal")
        //     number = std::to_string(normalNr++);
        // else if (name == "texture_height")
        //     number = std::to_string(heightNr++);
        // now set the sampler to the correct texture unit
        name += number;
        // set uniform sampler to correct texture unit
        shader.setInt(name, i);
        // bind the texture
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }

    // draw the mesh(all the triangles)
    glBindVertexArray(m_VAO);
    // draw all the
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}



